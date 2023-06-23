#include <unordered_map>

#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/utils.h"

namespace linpipe::kbelik {

TypedValue::TypedValue() { }

TypedValue::TypedValue(TypedValueSubtype st, string type_value, string specifiers) {
  create(st, type_value, specifiers);
}

TypedValue::TypedValue(string subtype_string, string type_value) { 
  auto st = string_to_subtype(subtype_string);
  string specifiers = "";
  if (st == TypedValueSubtype::monolingualtext || st == TypedValueSubtype::quantity) {
    vector<string> splitted;
    split(subtype_string, ":", splitted);
    specifiers = splitted[1];
  }
  create(st, type_value, specifiers);
}

void TypedValue::create(TypedValueSubtype st, string type_value, string specifiers) {
  subtype = st;
  if (st == TypedValueSubtype::qid) {
    id = stoull(type_value.substr(1));
    generic_string_val = {type_value[0]};
    vector<string> splitted;
    split(type_value, ":", splitted);
    string rest = "";
    for (size_t i = 1; i < splitted.size(); ++i)
      rest += splitted[i];
    specifics = rest;
  }
  else if (st == TypedValueSubtype::quantity) {
    id = stoull(specifiers.substr(1));
    specifics = stod(type_value);
  }
  else if (st == TypedValueSubtype::monolingualtext) {
    specifics = specifiers;
    generic_string_val = type_value;
  }
  else 
    generic_string_val = type_value;
}

pair<string, string> TypedValue::get_as_string() const {
  string val = get_val();
  if (subtype == TypedValueSubtype::monolingualtext) {
    return {subtype_to_string(subtype) + ':' + get<string>(specifics), val};
  }
  else if (subtype == TypedValueSubtype::quantity) {
    return {subtype_to_string(subtype) + ":Q" + to_string(id), to_string(get<double>(specifics)) };
  }
  return {subtype_to_string(subtype), val};
}

string TypedValue::get_val() const {
  if (subtype == TypedValueSubtype::qid)
    return generic_string_val + to_string(id) + ((get<string>(specifics).size() > 0) ? (":" + get<string>(specifics)) : "");
  else if (subtype == TypedValueSubtype::quantity)
    return to_string(get<double>(specifics));
  else
    return generic_string_val;
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
//    {"monolingualtext", TypedValueSubtype::monolingualtext},
    {"time:gregorian", TypedValueSubtype::time_gregorian},
    {"time:julian", TypedValueSubtype::time_julian},
//    {"quantity", TypedValueSubtype::quantity}
  };

  auto it = map.find(subtype);
  if (it != map.end()) 
    return it->second;
  else if (subtype.find("quantity") != string::npos)
    return TypedValueSubtype::quantity;
  else if (subtype.find("monolingualtext") != string::npos)
    return TypedValueSubtype::monolingualtext;

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

/*
void TypedValue::from_string_representation(string data) {
  vector<string> splitted;
  size_t start = 0;
  size_t end = 0;
  while((end = data.find(delimiter(), start)) != string::npos) {
    splitted.push_back(data.substr(start, end - start));
    start = end + delimiter().size();
  }
  splitted.push_back(data.substr(start));

  const int n_of_fields = 3;
  if (splitted.size() != n_of_fields)
    throw LinpipeError("The deserializaton of the string failed. Different number of serialized fields is expected.");
  id = stoull(splitted[0]);
  subtype = string_to_subtype(splitted[1]);
  val = splitted[2];
}

string TypedValue::to_string_representation() const {
  return to_string(id) + delimiter() + subtype_to_string(subtype) + delimiter() + val;
}*/

} // linpipe::kbelik
