#pragma once

#include "common.h"
#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/speicific_serializer.h"

using namespace linpipe {
using namespace kbelik {
  template<typename T>
  class SpecificKbelik{
    private:
      PersistentMap map_ids;
      PersistentMap map_atts;
      void load(string map_path);
    public:
      AgnosticKbelik(string map_path);
      T query_ids();
      T query_atts();
      void save();
      ~AgnosticKbelik();
  };
}
}
