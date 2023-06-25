#pragma once

#include <filesystem>

#include "lib/json.h"

#include "common.h"
//#include "dev/kbelik/id.h"
#include "dev/kbelik/named_entity.h"
#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/utils.h"

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
  vector<NamedEntity> named_entities;
  Ternary fictional;

  AgnosticEntityInfo();
  AgnosticEntityInfo(Json& js);
  
 private:
  void claims_from_wikidata_json(Json& clms);
  void fictional_from_wikidata_json(Json& ne);
  void ne_from_wikidata_json(Json& ne);

  static inline NamedEntity named_entity_from_string(const string& str);

  unordered_map<string, TypedValue> create_optionals(Json& js);
};

inline bool operator==(const AgnosticEntityInfo& a, const AgnosticEntityInfo& b) {
  return a.claims == b.claims && a.named_entities == b.named_entities && a.fictional == b.fictional;
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
