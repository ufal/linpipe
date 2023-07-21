#include <filesystem>
#ifdef _WIN32
  #include <Windows.h>
  #include <memoryapi.h>
  #include <winbase.h>
  #include <fileapi.h>
#else
  #include <sys/mman.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
#endif

#include "common.h"

#include "dev/kbelik/mmap.h"

namespace linpipe::kbelik {
#ifdef _WIN32
class MMappedFile::Impl {
 public:
  Impl(filesystem::path fp, size_t offset, int64_t len) {
    length = len;

    DWORD pagesize;

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    pagesize = si.dwPageSize;
    DWORD page_start = (offset / pagesize) * pagesize;
    DWORD page_offset = offset % pagesize;

    handle = CreateFile(fp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE)
      LinpipeError("open");
    if (length == -1) {
      LARGE_INTEGER file_size;
      if (!GetFileSizeEx(handle, &file_size))
        LinpipeError("stat");
      length = file_size.QuadPart - page_offset;
    }

    fileMappingHandle = CreateFileMapping(handle, NULL, PAGE_READONLY, 0, 0, NULL);
    if (fileMappingHandle == INVALID_HANDLE_VALUE)
      LinpipeError("CreateFileMapping");
    original_mmap_addr = MapViewOfFile(fileMappingHandle, FILE_MAP_READ, 0, page_start, length + page_offset);
    if (mmap_addr == NULL)
      LinpipeError("map");
    mmap_addr = static_cast<std::byte*>(original_mmap_addr) + page_offset;
  }

  bool opened() const {
    return handle != INVALID_HANDLE_VALUE;
  }

  void close() {
    if (mmap_addr != nullptr) {
      UnmapViewOfFile(original_mmap_addr);
      mmap_addr = nullptr;
      original_mmap_addr = nullptr;
    }
    if (fileMappingHandle != INVALID_HANDLE_VALUE) {
      CloseHandle(fileMappingHandle);
      fileMappingHandle = INVALID_HANDLE_VALUE;
    }
    if (handle != INVALID_HANDLE_VALUE) {
      CloseHandle(handle);
      handle = INVALID_HANDLE_VALUE;
    }
  }

  ~Impl() {
    close();
  }

  int64_t length;
  off_t page_offset;
  HANDLE handle;
  HANDLE fileMappingHandle;
  void* mmap_addr;
  void* original_mmap_addr;

};

#else

class MMappedFile::Impl {
 public:
  Impl(filesystem::path fp, size_t offset, int64_t len) {
    length = len;

    size_t pagesize = sysconf(_SC_PAGESIZE);
    off_t page_start = (offset / pagesize) * pagesize;
    page_offset = offset % pagesize;

    fd = open(fp.c_str(), O_RDONLY);
    struct stat sb;
    if (fd == -1)
      LinpipeError("open");
    if (fstat(fd, &sb) == -1)
      LinpipeError("stat");
    if (length == -1)
      length = sb.st_size - page_offset;

    mmap_addr = mmap(NULL, length + page_offset, PROT_READ, MAP_PRIVATE, fd, page_start);
    mmap_addr = static_cast<std::byte*>(mmap_addr) + page_offset;
    if (mmap_addr == MAP_FAILED)
      LinpipeError("map");
  }

  bool opened() const {
    struct stat file_stat;
    return fstat(fd, &file_stat) == 0;
  }

  void close() {
    munmap((byte*)mmap_addr - page_offset, length + page_offset);
    mmap_addr = NULL;
    ::close(fd);
  }

  ~Impl() {
    if (opened())
      close();
  }

  int64_t length;
  off_t page_offset;
  int fd;
  void* mmap_addr;

};
#endif

MMappedFile::MMappedFile(filesystem::path fp, size_t offset, int64_t length) : pImpl(make_unique<Impl>(fp, offset, length)) {}

const void* MMappedFile::mmap_addr() const {
  return pImpl->mmap_addr;
}

int64_t MMappedFile::length() const {
  return pImpl->length;
}

bool MMappedFile::opened() const {
  return pImpl->opened();
}

void MMappedFile::close() {
  pImpl->close();
}

MMappedFile::~MMappedFile() = default;


/*
class MMappedFileUnix : public MMappedFile {
 public:
  MMappedFileUnix(size_t page_offset, int64_t length, const void* data, int fd) : MMappedFile(data, length, page_offset), _fd(fd) {}

  bool opened() override {
    struct stat file_stat;
    return fstat(_fd, &file_stat) == 0;
  }

  void close() override {
    munmap((byte*)_data - _page_offset, _length);
    _data = NULL;
    ::close(_fd);
  }

  static unique_ptr<MMappedFile> create(filesystem::path path, size_t offset, int64_t length) {
    size_t pagesize = sysconf(_SC_PAGESIZE);
    off_t page_start = (offset / pagesize) * pagesize;
    off_t page_offset = offset % pagesize;

    int fd = open(path.c_str(), O_RDONLY);
    struct stat sb;
    if (fd == -1)
      LinpipeError("open");
    if (fstat(fd, &sb) == -1)
      LinpipeError("stat");
    if (length == -1)
      length = sb.st_size;

    void* mmap_addr = mmap(NULL, length + page_offset, PROT_READ, MAP_PRIVATE, fd, page_start);
    mmap_addr = static_cast<std::byte*>(mmap_addr) + page_offset;
    if (mmap_addr == MAP_FAILED)
      LinpipeError("map");
    return make_unique<MMappedFileUnix>(page_offset, length, mmap_addr, fd);
  }
 private:
  int _fd;
};
 */

/*
MMappedFile::MMappedFile(filesystem::path fp, size_t offset, int64_t length) {
  size_t pagesize;

#ifdef _WIN32
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  pagesize = si.dwPageSize;
#else
  pagesize = sysconf(_SC_PAGESIZE);
#endif

  off_t page_start = (offset / pagesize) * pagesize;
  off_t page_offset = offset % pagesize;

#ifdef _WIN32
  fd = CreateFile(fp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (fd == INVALID_HANDLE_VALUE)
    LinpipeError("open");
  if (length == -1) {
    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(fd, &file_size))
      LinpipeError("stat");
    length = file_size.QuadPart;
  }

  mmap_addr = MapViewOfFile(fd, FILE_MAP_READ, 0, page_start, length + page_offset);
  mmap_addr = static_cast<std::byte*>(mmap_addr) + page_offset;
  if (mmap_addr == NULL)
    LinpipeError("map");
  
#else
  
#endif
}

void MMappedFile::close() {
  if (opened()) {
#ifdef _WIN32
    UnmapViewOfFile(mmap_addr);
    mmap_addr = NULL;
    CloseHandle(fd);
#else
    munmap(mmap_addr, length);
    mmap_addr = NULL;
    ::close(fd);
#endif
  }
}

bool MMappedFile::opened() const {
#ifdef _WIN32
  return fd != INVALID_HANDLE_VALUE;
#else
  struct stat file_stat;
  return fstat(fd, &file_stat) == 0;
#endif
}

MMappedFile::~MMappedFile() {
  if (opened())
    close();
} 
*/

} // namespace linpipe::kbelik
