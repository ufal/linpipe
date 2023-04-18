#pragma once

#include <filesystem>
#include "common.h"

namespace linpipe::kbelik {

template<typename Key, typename Value>
class PersistentMap{
 public:
  PersistentMap(filesystem::path fp, size_t offset=0, size_t length=-1);
  ~PersistentMap();

  bool find(Key key, Value::Type& value) const;
  void close(); // Should end mmap.

 private:
  int length = 0; 
#ifdef _WIN_32
  // TODO
#else
  int fd = 0;
  void* mmap_addr = NULL;
#endif

  void load(filesystem::path fp);
};

} // namespace linpipe::kbelik
