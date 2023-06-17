#pragma once

#include <filesystem>

#include "lib/json.h"

#include "common.h"
//#include "dev/kbelik/id.h"
#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/typed_value.h"

namespace linpipe::kbelik {

class AgnosticEntityInfo {
 public:
  map<string, TypedValue> claims;

  AgnosticEntityInfo() {
    claims = map<string, TypedValue>();
  }

  AgnosticEntityInfo(Json& clms) {
    from_wikidata_json(clms);
  }
  
  void from_wikidata_json(Json& clms) {
    for(auto& [key, val] : clms.items()) {
      string sub_type = val.at(0).at(0);
      string type_value = val.at(0).at(1);
      claims[key] = TypedValue(sub_type, type_value);
    }
  }
};

/*
class AgnosticKbelik {
 public:
  // Rovnou map_path
  AgnosticKbelik(filesystem::path map_path, size_t offset=0, size_t length=-1);
  ~AgnosticKbelik();

  bool find(ID id, AgnosticEntityInfo& value) const;
  void close(); // Should end mmap.

  static void build(istream& json);
 private:
  // Chápe PM formát dat?
  PersistentMap<ID, map_values::AgnosticEntityInfo> map;
  void load (filesystem::path map_path);
};
*/
} // namespace linpipe::kbelik
