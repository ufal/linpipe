#include <unordered_map>

#include "dev/kbelik/typed_value.h"

namespace linpipe::kbelik {

TypedValue::TypedValue() { }

TypedValue::TypedValue(TypedValueSubtype st, string type_value) {
  subtype = st;
  if (st == TypedValueSubtype::qid) {
    id = stoull(type_value.substr(1));
    val = {type_value[0]};
  }
  else 
    val = type_value;
}

TypedValue::TypedValue(string st, string type_value) : TypedValue(string_to_subtype(st), type_value) { }


string TypedValue::get_val() const {
  if (subtype != TypedValueSubtype::qid)
    return val;
  else
    return val + to_string(id);  
}
    
TypedValueSubtype TypedValue::get_subtype () const {
  return subtype;
}


TypedValueSubtype TypedValue::string_to_subtype(const string& subtype) {
  static const unordered_map<string, TypedValueSubtype> map {
    {"qid", TypedValueSubtype::qid},
    {"url", TypedValueSubtype::url},
    {"string", TypedValueSubtype::string},
    {"external-id", TypedValueSubtype::external_id},
    {"wikibase-item", TypedValueSubtype::wikibase_item},
    {"wikibase-property", TypedValueSubtype::wikibase_property},
    {"commonsMedia", TypedValueSubtype::commonsMedia},
    {"geo-shape", TypedValueSubtype::geo_shape},
    {"math", TypedValueSubtype::math},
    {"musical-notation", TypedValueSubtype::musical_notation},
    {"tabular-data", TypedValueSubtype::tabular_data},
    {"wikibase-lexeme", TypedValueSubtype::wikibase_lexeme},
    {"wikibase-form", TypedValueSubtype::wikibase_form},
    {"wikibase-sense", TypedValueSubtype::wikibase_sense},
    {"globe-coordinate", TypedValueSubtype::globe_coordinates},
    {"monolingualtext", TypedValueSubtype::monolingualtext},
    {"time:gregorian", TypedValueSubtype::time_gregorian},
    {"time:julian", TypedValueSubtype::time_julian},
    {"quantity", TypedValueSubtype::quantity}
  };

  auto it = map.find(subtype);
  if (it != map.end()) {
    return it->second;
  }

  throw LinpipeError("Unknown subtype: " + subtype);
}

string TypedValue::subtype_to_string(const TypedValueSubtype& subtype) {
  static const unordered_map<TypedValueSubtype, string> map {
    {TypedValueSubtype::qid, "qid"},
    {TypedValueSubtype::url, "url"},
    {TypedValueSubtype::string, "string"},
    {TypedValueSubtype::external_id, "external-id"},
    {TypedValueSubtype::wikibase_item, "wikibase-item"},
    {TypedValueSubtype::wikibase_property, "wikibase-property"},
    {TypedValueSubtype::commonsMedia, "commonsMedia"},
    {TypedValueSubtype::geo_shape, "geo-shape"},
    {TypedValueSubtype::math, "math"},
    {TypedValueSubtype::musical_notation, "musical-notation"},
    {TypedValueSubtype::tabular_data, "tabular-data"},
    {TypedValueSubtype::wikibase_lexeme, "wikibase-lexeme"},
    {TypedValueSubtype::wikibase_form, "wikibase-form"},
    {TypedValueSubtype::wikibase_sense, "wikibase-sense"},
    {TypedValueSubtype::globe_coordinates, "globe-coordinate"},
    {TypedValueSubtype::monolingualtext, "monolingualtext"},
    {TypedValueSubtype::time_gregorian, "time:gregorian"},
    {TypedValueSubtype::time_julian, "time:julian"},
    {TypedValueSubtype::quantity, "quantity"}
  };

  auto it = map.find(subtype);
  if (it != map.end())
    return it->second;
  
  throw LinpipeError("Unknown subtype");
}

} // linpipe::kbelik
