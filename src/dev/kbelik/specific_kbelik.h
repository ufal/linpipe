#pragma once

#include <filesystem>
#include "common.h"
#include "dev/kbelik/persistent_map.h"

namespace linpipe {
namespace kbelik {
  class SpecificKbelik{
    public:
      SpecificKbelik(filesystem::path map_path);
      ~SpecificKbelik();

      static void build(istream json);
      Value find(Key id);
      void save();

    private:
      PersistentMap<Key, Value> ids_to_atts;
      void load(string map_path);
  };
}
}

// - Values jsou
//   - label: str
//   - aliases: list[str]
//   - description: str
//   - text: str (je ve "wiki"/"text", ale "wiki" tam nemusí být
