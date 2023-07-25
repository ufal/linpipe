#pragma once

#include <cstring>
#include <filesystem>
#include <fstream>

#include "common.h"

#include "dev/kbelik/map_type.h"
#include "dev/kbelik/mmap.h"

#include "dev/kbelik/utils/memcpy.h"

namespace linpipe::kbelik {

template<typename MapKey, typename Value>
class PersistentMap{
 public:
  PersistentMap(filesystem::path fp, MapKey mk, Value mv, size_t offset=0, int64_t length=-1) : 
  mk(mk), mv(mv), mf(MMappedFile(fp, offset, length)) {
    init();
  }
  ~PersistentMap();

  bool find(typename MapKey::Type key, typename Value::Type& value) const;
  void close(); // Should end mmap.
                
  bool opened() const;

  MapType get_map_type() const;

  MapKey mk;
  Value mv;
 private:
  MapType map_type;
  MMappedFile mf;
  std::byte* index_start;
  size_t index_size;

  size_t one_key; // Size of one key entry in the index.
  int jump_exponential; // Jump for the exponential search.

  void load(filesystem::path fp, size_t offset, int64_t length);
  void init(); // Only init that is needed, calls other inits and should be called from load.
  void init_index_ptr();
  void set_map_type();

  bool get_val_offset(uint64_t key, uint32_t& offset) const;

  bool exponential_search(uint64_t key, size_t elements_cnt, uint32_t &result) const;
  bool binary_search(uint64_t key, size_t l, size_t r, uint32_t &result) const;

  void read_ith_key(int i, uint64_t &res) const;
  void read_ith_offset(int i, uint32_t &res) const;

};

/*
template<typename MapKey, typename Value>
PersistentMap<MapKey, Value>::PersistentMap(filesystem::path fp, Value& mv, size_t offset, int64_t length, ByteSerializerDeserializers* bsds) {
  this->mv = mv;
  this->bsds = bsds;
  load(fp, offset, length);
}*/

template<typename MapKey, typename Value>
bool PersistentMap<MapKey, Value>::find(typename MapKey::Type key, typename Value::Type& value) const {
  if (!opened())
    throw LinpipeError("The mmap is closed.");
  uint32_t offset_in_map;
  uint64_t key_as_uint = mk.convert_to_uint(key);
  bool success = get_val_offset(key_as_uint, offset_in_map);
  if (success) {
    const byte* ptr = static_cast<std::byte*>(index_start) + offset_in_map;
    mv.deserialize(ptr, value);
  }
  return success;
}

template<typename MapKey, typename Value>
void PersistentMap<MapKey, Value>::close() {
  mf.close();
}

template<typename MapKey, typename Value>
bool PersistentMap<MapKey, Value>::opened() const {
  return mf.opened();
}

template<typename MapKey, typename Value>
MapType PersistentMap<MapKey, Value>::get_map_type() const {
  return map_type;
}

/*
template<typename MapKey, typename Value>
void PersistentMap<MapKey, Value>::load(filesystem::path fp, size_t offset, int64_t length) {

  size_t pagesize;
#ifdef _WIN_32
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  pagesize = si.dwPageSize;
#else
  pagesize = sysconf(_SC_PAGESIZE);
#endif

  off_t page_start = (offset / pagesize) * pagesize;
  off_t page_offset = offset % pagesize;

#ifdef _WIN_32
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
  fd = open(fp.c_str(), O_RDONLY);
  struct stat sb;
  if (fd == -1)
    LinpipeError("open");
  if (fstat(fd, &sb) == -1)
    LinpipeError("stat");
  if (length == -1)
    length = sb.st_size;

  mmap_addr = mmap(NULL, length + page_offset, PROT_READ, MAP_PRIVATE, fd, page_start);
  mmap_addr = static_cast<std::byte*>(mmap_addr) + page_offset;
  if (mmap_addr == MAP_FAILED)
    LinpipeError("map");
#endif
}
*/

template<typename MapKey, typename Value>
void PersistentMap<MapKey, Value>::init() {
  init_index_ptr();
  set_map_type();
  if (map_type == test) {
    one_key = 4 + mk.length();
    jump_exponential = 8;
  }
}

template<typename MapKey, typename Value>
void PersistentMap<MapKey, Value>::init_index_ptr() {
  index_start = (std::byte*)(mf.mmap_addr()) + sizeof(size_t) + sizeof(MapType);
  memcpy(&index_size, (std::byte*)(mf.mmap_addr()) + sizeof(MapType), sizeof(size_t));
}

template<typename MapKey, typename Value>
void PersistentMap<MapKey, Value>::set_map_type() {
  memcpy(&map_type, (std::byte*)mf.mmap_addr(), sizeof(MapType));
}

template<typename MapKey, typename Value>
bool PersistentMap<MapKey, Value>::get_val_offset(uint64_t key, uint32_t& offset) const {
  return exponential_search(key, index_size / one_key, offset);
}

template<typename MapKey, typename Value>
bool PersistentMap<MapKey, Value>::binary_search(uint64_t key, size_t l, size_t r, uint32_t &result) const {
  while(l <= r) {
    size_t m = (l + r) / 2;
    uint64_t from_index;
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

template<typename MapKey, typename Value>
bool PersistentMap<MapKey, Value>::exponential_search(uint64_t key, size_t elements_cnt, 
                                                   uint32_t &result) const {
  size_t r = 1;
  while (r < elements_cnt) {
    uint64_t from_index;
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

template<typename MapKey, typename Value>
void PersistentMap<MapKey, Value>::read_ith_key(int i, uint64_t &res) const {
  size_t shift = i * one_key;
  // mk.deserialize(index_start + shift, res);
  utils::load_n_byte_uint(index_start + shift, res, mk.length());
  //memcpy(&res, index_start + shift , sizeof(MapKey));
}

template<typename MapKey, typename Value>
void PersistentMap<MapKey, Value>::read_ith_offset(int i, uint32_t &res) const {
  size_t shift = i * one_key;
  memcpy(&res, index_start + shift + mk.length(), sizeof(uint32_t));
  res += shift;
}

template<typename MapKey, typename Value>
PersistentMap<MapKey, Value>::~PersistentMap() {
  if(opened())
    close();
}

} // namespace linpipe::kbelik
