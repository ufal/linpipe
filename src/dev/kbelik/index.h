#pragma once

#include "common.h"

using namespace linpipe {
using namespace kbelik {
  template<typename T>
  class Index {
    private:
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
