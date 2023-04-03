#include <filesystem>
#include "common.h"
#include "dev/kbelik/persistent_map.h"
#include <iostream>
#include <fstream>
#ifdef _WIN_32
      // TODO
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif


namespace linpipe {
namespace kbelik {

template<typename Key, typename Value>
PersistentMap<Key, Value>::PersistentMap(filesystem::path fp) {
  load(fp);
}

template<typename Key, typename Value>
PersistentMap<Key, Value>::PersistentMap() {
}

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

}
}
