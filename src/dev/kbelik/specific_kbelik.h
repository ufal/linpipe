#pragma once

#include <filesystem>
#include "common.h"
#include "dev/kbelik/persistent_map.h"

namespace linpipe {
namespace kbelik {
  template<typename Key, typename Value>
  class SpecificKbelik{
    private:
      PersistentMap<Key, Value> ids_to_atts;
      PersistentMap<Value, Key> atts_to_ids; // Maybe we need to use something different than PersistentMap
      void load(string map_path);
    public:
      SpecificKbelik(filesystem::path map_path);
      static void build(istream json);
      Value find_att(Key id);
      Key find_id(Value att);
      void save();
      ~SpecificKbelik();
  };
}
}
