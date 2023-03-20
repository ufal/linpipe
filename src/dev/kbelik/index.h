#pragma once

#include "common.h"

using namespace linpipe {
using namespace kbelik {
  template<typename T>
  class Index {
    private:
      int fd;   // TODO check for portability
      int length; 
      const void* mmap_addr;
      void load(string fp);
      vector<T> qids_from_wikidata(string fp);
    public:
      Index(string fp);
      ~Index();
      void save(string fp) const;
      T find(T key) const;
      void build(string fp);
  }
}
}
