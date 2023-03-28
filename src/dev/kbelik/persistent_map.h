#pragma once

#include <filesystem>
#include "common.h"

namespace linpipe {
namespace kbelik {
  template<typename Key, typename Value>
  class PersistentMap{
    private:
#ifdef _WIN_32
      // TODO
#else
      int fd = 0;
      int length = 0; 
      void* mmap_addr = NULL;
#endif
      void load(filesystem::path fp);
     // vector<int> qids_from_wikidata(filesystem::path fp);
    public:
      static size_t get_val_space(map<Key, Value>* data);
      PersistentMap(filesystem::path fp);
      PersistentMap();
      ~PersistentMap();
      int find(Key key) const;
      static void build(map<Key, Value>* data, filesystem::path path);
      void close(); // Should end mmap.
  };
}
}
