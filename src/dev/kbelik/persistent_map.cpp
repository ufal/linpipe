#include <filesystem>
#include <fstream>
#include <iostream>
#include <string.h>
#ifdef _WIN_32
      // TODO
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "common.h"
#include "dev/kbelik/persistent_map.h"

namespace linpipe {
namespace kbelik {

template<typename Key, typename Value>
PersistentMap<Key, Value>::PersistentMap(filesystem::path fp, size_t offset, size_t length) {
  load(fp, offset, length);
  init_index_ptr();
  opened = true;
}

template<typename Key, typename Value>
void PersistentMap<Key, Value>::load(filesystem::path fp, size_t offset, size_t length) {
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

/*

template<typename Key, typename Value>
void PersistentMap<Key, Value>::load(filesystem::path fp) {
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
void PersistentMap<Key, Value>::close(){
  munmap(mmap_addr, length);
  ::close(fd);
}

template<typename Key, typename Value>
size_t PersistentMap<Key, Value>::get_val_space(map<Key, Value>* data) {
  size_t val_space = 0;
  for (auto &p : *data) {
    val_space += p.second.size();
  }
  return val_space;
}

template<typename Key, typename Value>
void PersistentMap<Key, Value>::save_map(byte* for_search, size_t size, filesystem::path path) {
  cout << "path: " << path.c_str()<<endl;
  ofstream f(path, ios::out | ios::trunc | ios::binary);
  if (f.is_open())
    f.write((char*)for_search, size);
  f.close();
}

template<typename Key, typename Value>
void PersistentMap<Key, Value>::build(map<Key, Value>* data, filesystem::path path) {
  byte* for_search;
  size_t key_space = (*data).size() * 2 * sizeof(Key);
  size_t val_space = get_val_space(data);
  //for_search = vector<byte>(key_space + val_space, byte{0});
  size_t for_search_size = key_space + val_space;
  for_search = new byte[for_search_size];
  byte* key_pos, *val_pos;
  key_pos = for_search;
  val_pos = (for_search + key_space);
  for (auto &p : *data) {
    int key = p.first;
    for (size_t i = 0; i < sizeof(Key); ++i) {
      *key_pos = static_cast<byte>(key >> (8 * i));
      //cout << (int)*key_pos << endl;
      key_pos++;
    }
    for (size_t i = sizeof(Key); i < 2 * sizeof(Key); ++i) {
      *key_pos = static_cast<byte>((val_pos - key_pos) >> (8 * i));
      key_pos++;
    }
    for (auto p: *data) {
      for (auto dato : p.second) {
        *val_pos = dato;
        val_pos++;
      }
    }
  }
  save_map(for_search, for_search_size, path);
}

template<typename Key, typename Value>
PersistentMap<Key, Value>::~PersistentMap() {
  close();
}
*/

} // namespace linpipe
} // namespace kbelik
