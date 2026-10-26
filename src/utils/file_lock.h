// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <filesystem>

#include "common.h"

namespace linpipe {

class FileLock {
 public:
  explicit FileLock(const std::filesystem::path& path);
  ~FileLock();

  FileLock(const FileLock&) = delete;
  FileLock& operator=(const FileLock&) = delete;

 private:
#ifdef _WIN32
  void* handle_;
#else
  int fd_;
#endif
};

} // namespace linpipe
