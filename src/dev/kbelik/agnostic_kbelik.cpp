#include "common.h"

#include "dev/kbelik/agnostic_kbelik.h"

namespace linpipe::kbelik {

AgnosticEntityInfo::AgnosticEntityInfo() {
  claims = unordered_map<string, AEIProperties>();
}

AgnosticEntityInfo::AgnosticEntityInfo(Json& clms) {
  from_wikidata_json(clms);
}

void AgnosticEntityInfo::from_wikidata_json(Json& clms) {
  for(auto& [key, val] : clms.items()) {
    string sub_type = val.at(0).at(0);
    string type_value = val.at(0).at(1);
    auto optionals = create_optionals(val.at(0).at(2));
    claims[key] = {TypedValue(sub_type, type_value), optionals};
  }
}

unordered_map<string, TypedValue> AgnosticEntityInfo::create_optionals(Json& js) {
  unordered_map<string, TypedValue> optionals;
  for(auto& [key, val] : js.items()) {
    string sub_type = val.at(0).at(0);
    string type_value = val.at(0).at(1);
    optionals[key] = TypedValue(sub_type, type_value);
  }
  return optionals;
}

} // linpipe::kbelik