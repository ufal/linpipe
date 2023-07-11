#pragma once

#include "common.h"

#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/named_entity.h"
#include "dev/kbelik/utils.h"

namespace linpipe::kbelik {

struct AEIProperties {
  TypedValue tv;
  unordered_map<string, vector<TypedValue>> optionals;
  //unordered_map<string, TypedValue> optionals;
};

inline bool operator==(const AEIProperties& a, const AEIProperties& b) {
  return a.tv == b.tv && a.optionals == b.optionals;
}

inline bool operator!=(const AEIProperties& a, const AEIProperties& b) {
  return !(a == b);
}


class AgnosticEntityInfo {
 public:
  unordered_map<string, vector<AEIProperties>> claims;
  //unordered_map<string, AEIProperties> claims;
  // Might be better to use un_set:
  vector<NamedEntity> named_entities;
  Ternary fictional;

  AgnosticEntityInfo();
  AgnosticEntityInfo(Json& js);
  
 private:
  void claims_from_wikidata_json(Json& clms);
  void fictional_from_wikidata_json(Json& ne);
  void ne_from_wikidata_json(Json& ne);

  static inline NamedEntity named_entity_from_string(const string& str);

  unordered_map<string, vector<TypedValue>> create_optionals(Json& js);
};

inline bool operator==(const AgnosticEntityInfo& a, const AgnosticEntityInfo& b) {
  return a.claims == b.claims && a.named_entities == b.named_entities && a.fictional == b.fictional;
}

inline bool operator!=(const AgnosticEntityInfo& a, const AgnosticEntityInfo& b) {
  return !(a == b);
}

} // namespace linpipe::kbelik