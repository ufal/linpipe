#pragma once

#include "common.h"

#include <lib/json.h>

namespace linpipe::kbelik {

enum class TypedValueSubtype {
  qid,
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
  time,
  quantity
};

class TypedValue {
  public:
    TypedValue();  // Needs default constructor so it works with map.
    TypedValue(TypedValueSubtype st, string type_value);
    TypedValue(string st, string type_value);

    string get_val() const;
    TypedValueSubtype get_subtype() const;

    static TypedValueSubtype string_to_subtype(const string& subtype);
    static string subtype_to_string(const TypedValueSubtype& subtype);

  private:
    string val;
    TypedValueSubtype subtype;
    uint64_t id; 
};


inline void to_json(Json &j, const TypedValue &tv) {
  j["val"] = tv.get_val();
  j["subtype"] = TypedValue::subtype_to_string(tv.get_subtype());
}

inline void from_json(const Json &j, TypedValue &tv) {
  string type_value = j["val"];
  string subtype = j["subtype"];
  tv = TypedValue(subtype, type_value);
}

inline bool operator==(const TypedValue& a, const TypedValue& b)
{
  return a.get_subtype() == b.get_subtype() && a.get_val() == b.get_val();
}


}  // linpipe:kbelik
