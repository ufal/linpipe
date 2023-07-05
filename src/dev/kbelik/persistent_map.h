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

#include "dev/kbelik/map_type.h"

namespace linpipe::kbelik {

template<typename KeyMV, typename Value>
class PersistentMap{
 public:
  PersistentMap(filesystem::path fp, size_t offset=0, size_t length=-1);
  ~PersistentMap();

  bool find(typename KeyMV::Type key, typename Value::Type& value) const;
  void close(); // Should end mmap.
                
  bool opened() const;

  MapType get_map_type() const;

 private:
  size_t length = 0; 
  MapType map_type;
#ifdef _WIN_32
  // TODO
#else
  int fd = -1;
  void* mmap_addr = NULL;
#endif
  byte* index_start;
  size_t index_size;

  size_t one_key; // Size of one key entry in the index.
  int jump_exponential; // Jump for the exponential search.

  void load(filesystem::path fp, size_t offset, size_t length);
  void init(); // Only init that is needed, calls other inits and should be called from load.
  void init_index_ptr();
  void set_map_type();

  bool get_val_offset(typename KeyMV::Type& key, uint32_t& offset) const;

  bool exponential_search(typename KeyMV::Type& key, size_t elements_cnt, uint32_t &result) const;
  bool binary_search(typename KeyMV::Type& key, size_t l, size_t r, uint32_t &result) const;

  void read_ith_key(int i, typename KeyMV::Type &res) const;
  void read_ith_offset(int i, uint32_t &res) const;

};

template<typename KeyMV, typename Value>
PersistentMap<KeyMV, Value>::PersistentMap(filesystem::path fp, size_t offset, size_t length) {
  load(fp, offset, length);
  init();
}

template<typename KeyMV, typename Value>
bool PersistentMap<KeyMV, Value>::find(typename KeyMV::Type key, typename Value::Type& value) const {
  if (!opened())
    throw LinpipeError("The mmap is closed.");
  uint32_t offset;
  bool success = get_val_offset(key, offset);
  if (success)
    Value::deserialize(static_cast<byte*>(index_start) + offset, value);
  return success;
}

template<typename KeyMV, typename Value>
void PersistentMap<KeyMV, Value>::close() {
  munmap(mmap_addr, length);
  ::close(fd);
}

template<typename KeyMV, typename Value>
bool PersistentMap<KeyMV, Value>::opened() const {
#ifdef _WIN_32
  // TODO
#else
  struct stat file_stat;
  return fstat(fd, &file_stat) == 0;
#endif
}

template<typename KeyMV, typename Value>
MapType PersistentMap<KeyMV, Value>::get_map_type() const {
  return map_type;
}

template<typename KeyMV, typename Value>
void PersistentMap<KeyMV, Value>::load(filesystem::path fp, size_t /*offset*/, size_t length) {
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

template<typename KeyMV, typename Value>
void PersistentMap<KeyMV, Value>::init() {
  init_index_ptr();
  set_map_type();
  if (map_type == test) {
    one_key = 4 + 8;
    jump_exponential = 8;
  }
}

template<typename KeyMV, typename Value>
void PersistentMap<KeyMV, Value>::init_index_ptr() {
  index_start = (byte*)(mmap_addr) + sizeof(size_t) + sizeof(MapType);
  memcpy(&index_size, (byte*)(mmap_addr) + sizeof(MapType), sizeof(size_t));
}

template<typename KeyMV, typename Value>
void PersistentMap<KeyMV, Value>::set_map_type() {
  memcpy(&map_type, (byte*)mmap_addr, sizeof(MapType));
}

template<typename KeyMV, typename Value>
bool PersistentMap<KeyMV, Value>::get_val_offset(typename KeyMV::Type& key, uint32_t& offset) const {
  return exponential_search(key, index_size / one_key, offset);
}

template<typename KeyMV, typename Value>
bool PersistentMap<KeyMV, Value>::binary_search(typename KeyMV::Type& key, size_t l, size_t r, uint32_t &result) const {
  while(l <= r) {
    size_t m = (l + r) / 2;
    typename KeyMV::Type from_index;
    read_ith_key(m, from_index);
    if (from_index < key) 
      l = m + 1;
    else if (from_index > key) {
      if (m == 0) // Prevents underflow and crash.
        return false;
      r = m - 1;
    }
    else {
      read_ith_offset(m, result);
      return true;
    }
  }
  return false;
}

template<typename KeyMV, typename Value>
bool PersistentMap<KeyMV, Value>::exponential_search(typename KeyMV::Type& key, size_t elements_cnt, 
                                                   uint32_t &result) const {
  size_t r = 1;
  while (r < elements_cnt) {
    typename KeyMV::Type from_index;
    read_ith_key(r, from_index);
    if (from_index < key)
      r *= jump_exponential;
    else if (from_index > key) {
      size_t l = r / jump_exponential;
      return binary_search(key, l, min(elements_cnt - 1, r), result);
    }
    else {
      read_ith_offset(r, result);
      return true;
    }
  }
  size_t l = r / jump_exponential;
  return binary_search(key, l, min(elements_cnt - 1, r), result);
}

template<typename KeyMV, typename Value>
void PersistentMap<KeyMV, Value>::read_ith_key(int i, typename KeyMV::Type &res) const {
  size_t shift = i * one_key;
  KeyMV::deserialize(index_start + shift, res);
  //memcpy(&res, index_start + shift , sizeof(KeyMV));
}

template<typename KeyMV, typename Value>
void PersistentMap<KeyMV, Value>::read_ith_offset(int i, uint32_t &res) const {
  size_t shift = i * one_key;
  memcpy(&res, index_start + shift + 8, sizeof(uint32_t));
  res += shift;
}

template<typename KeyMV, typename Value>
PersistentMap<KeyMV, Value>::~PersistentMap() {
  if (opened())
    close();
}

} // namespace linpipe::kbelik
