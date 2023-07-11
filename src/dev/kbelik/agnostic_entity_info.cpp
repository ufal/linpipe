
#include "common.h"

#include "dev/kbelik/agnostic_entity_info.h"

namespace linpipe::kbelik {

AgnosticEntityInfo::AgnosticEntityInfo() {
  claims = unordered_map<string, vector<AEIProperties>>();
}

AgnosticEntityInfo::AgnosticEntityInfo(Json& js) {
  claims_from_wikidata_json(js["claims"]);
  fictional_from_wikidata_json(js["named_entities"]);
  ne_from_wikidata_json(js["named_entities"]);
}

void AgnosticEntityInfo::claims_from_wikidata_json(Json& clms) {
  for(auto& [key, val] : clms.items()) {
    vector<AEIProperties> claim;
    for (auto &item : val) {
      string sub_type = item[0];
      string type_value = item[1];
      Json raw_optionals = item[2];
      auto optionals = create_optionals(raw_optionals);
      claim.push_back({TypedValue(sub_type, type_value), optionals});
    }
    claims[key] = claim;
  }
}

void AgnosticEntityInfo::fictional_from_wikidata_json(Json& ne) {
  fictional = Ternary::Maybe;
  if (ne.contains("fictional"))  
    fictional = ne["fictional"] ? Ternary::True : Ternary::False;
}

void AgnosticEntityInfo::ne_from_wikidata_json(Json& ne) {
  for (auto val : ne["type"])
    named_entities.push_back(named_entity_from_string(val));
}

unordered_map<string, vector<TypedValue>> AgnosticEntityInfo::create_optionals(Json& js) {
  unordered_map<string, vector<TypedValue>> optionals;
  for(auto& [key, val] : js.items()) {
    vector<TypedValue> optional;
    for (auto& item : val) {
      string sub_type = item[0];
      string type_value = item[1];
      optional.push_back(TypedValue(sub_type, type_value));
    }
    optionals[key] = optional;
  }
  return optionals;
}
  
NamedEntity AgnosticEntityInfo::named_entity_from_string(const string& str) {
  if (str == "PER") return NamedEntity::PER;
  else if (str == "ORG") return NamedEntity::ORG;
  else if (str == "LOC") return NamedEntity::LOC;
  else if (str == "EVENT") return NamedEntity::EVENT;
  else if (str == "BRAND") return NamedEntity::BRAND;
  else if (str == "WORK_OF_ART") return NamedEntity::WORK_OF_ART;
  else if (str == "MANUFACTURED") return NamedEntity::MANUFACTURED;
  else throw LinpipeError("Invalid string value for NamedEntity");
}

} // namespace linpipe::kbelik