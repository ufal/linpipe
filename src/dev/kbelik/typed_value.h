#pragma once

#include <variant>

#include "lib/json.h"

#include "common.h"

#include "dev/kbelik/id.h"

namespace linpipe::kbelik {

enum class TypedValueSubtype {
  id,
  url,
  string,
  external_id,
  wikibase_item,
  wikibase_property,
  commonsMedia,
  geo_shape,
  math,
  musical_notation,
  tabular_data,
  wikibase_lexeme,
  wikibase_form,
  wikibase_sense,
  globe_coordinates,
  monolingualtext,
  time_gregorian,
  time_julian,
  quantity
};

class TypedValue {
  public:
    TypedValue();  // Needs default constructor so it works with map.
    TypedValue(string type, string value);

    string get_string() const;
    string get_type_string() const;
    TypedValueSubtype get_type() const;

    ID get_id() const;
    string get_language() const;
    struct Time {
      int64_t year;
      int month;
      int day;
      string calendar;
    };
    Time get_time() const;
    struct Quantity {
      ID unit;
      string unit_label;
      double value;
    };
    Quantity get_quantity() const;

    static inline TypedValueSubtype string_to_subtype(const string& subtype);
    static inline string subtype_to_string(const TypedValueSubtype& subtype);

  private:
    string value;
    string type_string;
    TypedValueSubtype type;
};

TypedValueSubtype TypedValue::string_to_subtype(const string& subtype) {
  static const unordered_map<string, TypedValueSubtype> map {
    {"id", TypedValueSubtype::id},
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
    {TypedValueSubtype::id, "id"},
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


inline bool operator==(const TypedValue& a, const TypedValue& b) {
  return a.get_type_string() == b.get_type_string() && a.get_string() == b.get_string();
}

inline bool operator!=(const TypedValue& a, const TypedValue& b) {
  return !(a == b);
}

inline bool operator==(const TypedValue::Time& a, const TypedValue::Time& b) {
  return a.year == b.year && a.month == b.month && a.day == b.day && a.calendar == b.calendar;
}

inline bool operator!=(const TypedValue::Time& a, const TypedValue::Time& b) {
  return !(a == b);
}

inline bool operator==(const TypedValue::Quantity& a, const TypedValue::Quantity& b) {
  return a.unit == b.unit && a.unit_label == b.unit_label && a.value == b.value;
}

inline bool operator!=(const TypedValue::Quantity& a, const TypedValue::Quantity& b) {
  return !(a == b);
}

}  // linpipe:kbelik
