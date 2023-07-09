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
      void load(string map_path);
    public:
      SpecificKbelik(filesystem::path map_path);
      static void build(istream json);
      Value find_att(Key id);
      void save();
      ~SpecificKbelik();
  };
}
}

// - Values jsou
//   - label: str
//   - aliases: list[str]
//   - description: str
//   - text: str (je ve "wiki"/"text", ale "wiki" tam nemusí být
