#pragma once

#include <filesystem>
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

} // namespace linpipe::kbelik
