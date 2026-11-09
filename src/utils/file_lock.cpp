// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <system_error>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "utils/file_lock.h"
#include "utils/path_utf8.h"

namespace linpipe {

namespace {

std::string last_error_message() {
#ifdef _WIN32
  return std::error_code(static_cast<int>(GetLastError()), std::system_category()).message();
#else
  return std::error_code(errno, std::generic_category()).message();
#endif
}

} // namespace

FileLock::FileLock(const std::filesystem::path& path)
#ifndef _WIN32
    : path_(path)
#endif
{
#ifdef _WIN32
  for (DWORD delay_ms = 1;;) {
    handle_ = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE | DELETE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                          nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (handle_ == INVALID_HANDLE_VALUE) {
      // A file pending deletion cannot be opened until its last handle is closed, which is
      // reported as ERROR_ACCESS_DENIED; back off for up to ~2s before treating it as genuine.
      if (GetLastError() == ERROR_ACCESS_DENIED && delay_ms <= 1024) {
        Sleep(delay_ms);
        delay_ms *= 2;
        continue;
      }
      throw LinpipeError("Cannot open lock file {}: {}", path_to_utf8(path), last_error_message());
    }
    delay_ms = 1;

    OVERLAPPED overlapped{};
    if (!LockFileEx(handle_, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &overlapped)) {
      auto message = last_error_message();
      CloseHandle(handle_);
      throw LinpipeError("Cannot lock file {}: {}", path_to_utf8(path), message);
    }

    // Verify that the path still refers to the file we have locked.
    BY_HANDLE_FILE_INFORMATION locked, named;
    HANDLE probe = CreateFileW(path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    bool current = probe != INVALID_HANDLE_VALUE &&
        GetFileInformationByHandle(handle_, &locked) &&
        GetFileInformationByHandle(probe, &named) &&
        locked.dwVolumeSerialNumber == named.dwVolumeSerialNumber &&
        locked.nFileIndexHigh == named.nFileIndexHigh &&
        locked.nFileIndexLow == named.nFileIndexLow;
    if (probe != INVALID_HANDLE_VALUE)
      CloseHandle(probe);
    if (current)
      break;
    CloseHandle(handle_);
  }
#else
  for (;;) {
    do {
      fd_ = open(path.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, 0666);
    } while (fd_ < 0 && errno == EINTR);
    if (fd_ < 0)
      throw LinpipeError("Cannot open lock file {}: {}", path_to_utf8(path), last_error_message());

    int ret = 0;
    do {
      ret = flock(fd_, LOCK_EX);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
      auto message = last_error_message();
      close(fd_);
      throw LinpipeError("Cannot lock file {}: {}", path_to_utf8(path), message);
    }

    // Verify that the path still refers to the file we have locked.
    struct stat fd_stat, path_stat;
    if (fstat(fd_, &fd_stat) < 0) {
      auto message = last_error_message();
      close(fd_);
      throw LinpipeError("Cannot stat lock file {}: {}", path_to_utf8(path), message);
    }
    if (stat(path.c_str(), &path_stat) == 0) {
      if (fd_stat.st_dev == path_stat.st_dev && fd_stat.st_ino == path_stat.st_ino)
        break;
    } else if (errno != ENOENT) {
      auto message = last_error_message();
      close(fd_);
      throw LinpipeError("Cannot stat lock file {}: {}", path_to_utf8(path), message);
    }
    close(fd_);
  }
#endif
}

FileLock::~FileLock() {
#ifdef _WIN32
  FILE_DISPOSITION_INFO disposition{TRUE};  // delete the file on close
  SetFileInformationByHandle(handle_, FileDispositionInfo, &disposition, sizeof(disposition));
  OVERLAPPED overlapped{};
  UnlockFileEx(handle_, 0, MAXDWORD, MAXDWORD, &overlapped);
  CloseHandle(handle_);
#else
  unlink(path_.c_str());
  close(fd_);
#endif
}

} // namespace linpipe
