#pragma once

#include <variant>

#include "common.h"

#include "lib/json.h"

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
  time_gregorian,
  time_julian,
  quantity
};

class TypedValue {
  public:
    TypedValue();  // Needs default constructor so it works with map.
    TypedValue(TypedValueSubtype st, string type_value, string specifiers="");
    TypedValue(string st, string type_value);

    pair<string, string> get_as_string() const;
    string get_val() const;
    TypedValueSubtype get_subtype() const;

    static TypedValueSubtype string_to_subtype(const string& subtype);
    static string subtype_to_string(const TypedValueSubtype& subtype);

    //vector<byte> to_bytes() const;
    //void from_bytes(vector<byte>* in);

  private:
    string generic_string_val;
    TypedValueSubtype subtype;
    variant<double, string> specifics; 
    uint64_t id;

    void create(TypedValueSubtype st, string type_value, string specifiers);
};

/*
inline void to_json(Json &j, const TypedValue &tv) {
  j["val"] = tv.get_val();
  j["subtype"] = TypedValue::subtype_to_string(tv.get_subtype());
}

inline void from_json(const Json &j, TypedValue &tv) {
  string type_value = j["val"];
  string subtype = j["subtype"];
  tv = TypedValue(subtype, type_value);
}
*/

inline bool operator==(const TypedValue& a, const TypedValue& b) {
  auto ap = a.get_as_string();
  auto bp = b.get_as_string();
  return ap == bp;
}

inline bool operator!=(const TypedValue& a, const TypedValue& b) {
  return !(a == b);
}

}  // linpipe:kbelik
