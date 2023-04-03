#pragma once

#include <filesystem>
#include "common.h"
#include "dev/kbelik/persistent_map.h"

namespace linpipe {
namespace kbelik {
  template<typename Key, typename Value>
  class AgnosticKbelik{
    private:
      // Chápe PM formát dat?
      PersistentMap<Key, Value> map;
      void load (filesystem::path map_path);
    public:
      // Rovnou map_path
      AgnosticKbelik(filesystem::path map_path);
      Value find(Key id);
      static void build(istream json);
      ~AgnosticKbelik();
  };
}
}
