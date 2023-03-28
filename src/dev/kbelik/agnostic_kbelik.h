#pragma once

#include "common.h"
#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/agnostic_serializer.h"

using namespace linpipe {
using namespace kbelik {
  template<typename T>
  class AgnosticKbelik{
    private:
      PersistentMap map;
      void load(string map_path);
    public:
      AgnosticKbelik(string map_path);
      T query();
      static void build(istream json);
      ~AgnosticKbelik();
  };
}
}
