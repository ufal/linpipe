#pragma once

#include <filesystem>
#include "common.h"

namespace linpipe {
namespace kbelik {
//  template<typename T>
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
      vector<int> qids_from_wikidata(filesystem::path fp);
    public:
      byte* for_search;
      int get_val_space(map<int, vector<byte>>* data);
      PersistentMap(filesystem::path fp);
      PersistentMap();
      ~PersistentMap();
      int find(int key) const;
      void build(map<int, vector<byte>>* data);
      void close(); // Should end mmap.
  };
}
}
