#include <filesystem>
#include "common.h"
#include "dev/kbelik/persistent_map.h"
#ifdef _WIN_32
      // TODO
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif


namespace linpipe {
namespace kbelik {

PersistentMap::PersistentMap(filesystem::path fp) {
  load(fp);
}

PersistentMap::PersistentMap() {
}

void PersistentMap::load(filesystem::path fp) {
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

void PersistentMap::close(){
  munmap(mmap_addr, length);
  ::close(fd);
}

int PersistentMap::get_val_space(map<int, vector<byte>>* data) {
  int val_space = 0;
  for (auto &p : *data) {
    val_space += p.second.size();
  }
  return val_space;
}

void PersistentMap::build(map<int, vector<byte>>* data) {
  int key_space = (*data).size() * 2 * sizeof(int);
  int val_space = get_val_space(data);
  //for_search = vector<byte>(key_space + val_space, byte{0});
  for_search = new byte[key_space + val_space];
  byte* key_pos, *val_pos;
  key_pos = for_search;
  val_pos = (for_search + key_space);
  for (auto &p : *data) {
    int key = p.first;
    for (int i = 0; i < (int)sizeof(int); ++i) {
      *key_pos = static_cast<byte>(key >> (8 * i));
      cout << (int)*key_pos << endl;
      key_pos++;
    }
    for (int i = (int)sizeof(int); i < 2 * (int)sizeof(int); ++i) {
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
}

PersistentMap::~PersistentMap() {
  close();
}

}
}
