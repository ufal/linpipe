#pragma once

#include <filesystem>
#ifdef _WIN32
  typedef void* HANDLE;
#endif

#include "common.h"

namespace linpipe::kbelik {

/*
class MMappedFile{
 public:
  void* mmap_addr = NULL;

  MMappedFile() = delete;
  ~MMappedFile();

  virtual void close() = delete;
  virtual bool opened() const = delete;

 private:
  size_t length; 
#ifdef _WIN32
  HANDLE fd;
#else
  int fd = -1;
#endif
}; */

/*
class MMappedFile {
 public:
  virtual ~MMappedFile() {
    if (opened())
      close();
  }

  const void* mmap_addr() { return _data; }
  int64_t length() { return _length; }
  virtual bool opened();
  virtual void close();

  static unique_ptr<MMappedFile> create(filesystem::path path, size_t offset, int64_t length) = delete;
 protected:
  MMappedFile(const void* data, int64_t length, size_t page_offset) : _data(data), _length(length), _page_offset(page_offset) {};
  const void* _data;
  int64_t _length;
  size_t _page_offset;
};
*/
class MMappedFile {
 public:
  MMappedFile(filesystem::path fp, size_t offset, int64_t length);
  ~MMappedFile();

  const void* mmap_addr() const ;
  int64_t length() const ;
  bool opened() const;
  void close();

 private:
  class Impl;
  unique_ptr<Impl> pImpl;
};

} // namespace linpipe::kbelik