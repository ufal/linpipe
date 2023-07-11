#include <unordered_map>

#include "common.h"

#include "dev/kbelik/typed_value.h"

#include "dev/kbelik/id.h"
#include "dev/kbelik/utils.h"

namespace linpipe::kbelik {

TypedValue::TypedValue() { }

TypedValue::TypedValue(string type, string value) { 
  this->type = string_to_subtype(type);
  this->type_string = type;
  this->value = value;
}

string TypedValue::get_string() const {
  return value;
}

TypedValueSubtype TypedValue::get_type() const {
  return type;
}

string TypedValue::get_type_string() const {
  return type_string;
}

ID TypedValue::get_id() const {
  if (type != TypedValueSubtype::id)
    throw LinpipeError("TypedValue::get_id() called on non-id type");
  return ID(value);
}

string TypedValue::get_language() const {
  if (type != TypedValueSubtype::monolingualtext)
    throw LinpipeError("TypedValue::get_language() called on non-monolingualtext type");
  vector<string> splitted;
  split(type_string, ":", splitted);
  return splitted[1];
}

TypedValue::Time TypedValue::get_time() const {
  if (type != TypedValueSubtype::time_gregorian && type != TypedValueSubtype::time_julian)
    throw LinpipeError("TypedValue::get_time() called on non-time type");
  Time time;
  vector<string> splitted;
  split(value, "-", splitted);
  time.year = stoll(splitted[0]);
  time.month = stoi(splitted[1]);
  split(splitted[2], "T", splitted);
  time.day = stoi(splitted[0]);
  if (splitted[1] != "00:00:00Z")
    throw LinpipeError("TypedValue::get_time() called on non-midnight time (not supported)");

  split(type_string, ":", splitted);
  time.calendar = splitted[1];

  return time;
}

TypedValue::Quantity TypedValue::get_quantity() const {
  if (type != TypedValueSubtype::quantity)
    throw LinpipeError("TypedValue::get_quantity() called on non-quantity type");
  Quantity quantity;
  vector<string> splitted;
  split(type_string, ":", splitted);
  quantity.value = stod(value);
  quantity.unit = ID(splitted[1]);
  if (splitted.size() == 2)
    quantity.unit_label = "";
  else if (splitted.size() == 3)
    quantity.unit_label = splitted[2];
  return quantity;
}

} // linpipe::kbelik
