#pragma once

#include <filesystem>

#include "lib/json.h"

#include "common.h"
//#include "dev/kbelik/id.h"
#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/typed_value.h"

namespace linpipe::kbelik {

struct AEIProperties {
  TypedValue tv;
  unordered_map<string, TypedValue> optionals;
};

inline bool operator==(const AEIProperties& a, const AEIProperties& b) {
  return a.tv == b.tv && a.optionals == b.optionals;
}

inline bool operator!=(const AEIProperties& a, const AEIProperties& b) {
  return !(a == b);
}

class AgnosticEntityInfo {
 public:
  unordered_map<string, AEIProperties> claims;

  AgnosticEntityInfo();
  AgnosticEntityInfo(Json& clms);
  
 private:
  void from_wikidata_json(Json& clms);
  unordered_map<string, TypedValue> create_optionals(Json& js);
};

inline bool operator==(const AgnosticEntityInfo& a, const AgnosticEntityInfo& b) {
  return a.claims == b.claims;
}

inline bool operator!=(const AgnosticEntityInfo& a, const AgnosticEntityInfo& b) {
  return !(a == b);
}

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
