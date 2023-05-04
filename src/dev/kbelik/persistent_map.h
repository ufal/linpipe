#pragma once

#include <cstring>
#include <filesystem>
#include <fstream>
#ifdef _WIN_32
      // TODO
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "common.h"

namespace linpipe::kbelik {

template<typename Key, typename Value>
class PersistentMap{
 public:
  PersistentMap(filesystem::path fp, size_t offset=0, size_t length=-1);
  ~PersistentMap();

  bool find(Key key, typename Value::Type& value) const;
  void close(); // Should end mmap.

 private:
  size_t length = 0; 
  bool opened = false;
#ifdef _WIN_32
  // TODO
#else
  int fd = 0;
  void* mmap_addr = NULL;
#endif
  byte* index_start;
  size_t index_size;

  bool get_val_offset(Key key, uint32_t& offset) const;
  void init_index_ptr();
  void load(filesystem::path fp, size_t offset, size_t length);
};

template<typename Key, typename Value>
PersistentMap<Key, Value>::PersistentMap(filesystem::path fp, size_t offset, size_t length) {
  load(fp, offset, length);
  init_index_ptr();
  opened = true;
}

template<typename Key, typename Value>
void PersistentMap<Key, Value>::load(filesystem::path fp, size_t /*offset*/, size_t length) {
  fd = open(fp.c_str(), O_RDONLY);
  struct stat sb;
  if (fd == -1)
    LinpipeError("open");
  if (fstat(fd, &sb) == -1)
    LinpipeError("stat");
  length = sb.st_size;

  mmap_addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
  if (mmap_addr == MAP_FAILED)
    LinpipeError("map");
}

template<typename Key, typename Value>
void PersistentMap<Key, Value>::init_index_ptr() {
  index_start = static_cast<byte*>(mmap_addr) + sizeof(size_t) + sizeof(uint32_t);
  memcpy(&index_size, static_cast<byte*>(mmap_addr) + sizeof(uint32_t), sizeof(size_t));
}

template<typename Key, typename Value>
bool PersistentMap<Key, Value>::get_val_offset(Key key, uint32_t& offset) const{
  size_t one_key = 8;
  for (size_t shift = 0; shift < index_size; shift+=one_key) {
    Key from_index;
    memcpy(&from_index, index_start + shift, sizeof(Key));
    if (from_index == key) { 
      memcpy(&offset, index_start + shift + sizeof(Key), sizeof(uint32_t));
      offset += shift;
      return true;
    }
  }
  return false;
}

template<typename Key, typename Value>
bool PersistentMap<Key, Value>::find(Key key, typename Value::Type& value) const {
  if (!opened)
    throw LinpipeError("The mmap is closed.");
  uint32_t offset;
  bool success = get_val_offset(key, offset);
  if (success)
    Value::deserialize(static_cast<byte*>(index_start) + offset, value);
  return success;
}

template<typename Key, typename Value>
void PersistentMap<Key, Value>::close() {
  munmap(mmap_addr, length);
  ::close(fd);
  opened = false;
}

template<typename Key, typename Value>
PersistentMap<Key, Value>::~PersistentMap() {
  if (opened)
    close();
}


} // namespace linpipe::kbelik
