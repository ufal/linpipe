// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/json.h"
#include "utils/json_utils.h"

namespace linpipe {

Json json_parse(const string_view caller, const string_view input) {
  try {
    return Json::parse(input);
  } catch (Json::parse_error& error) {
    throw LinpipeError{caller, ": Cannot parse JSON: ", error.what()};
  }
}

void json_assert_object(const string_view caller, const Json& json) {
  if (!json.is_object())
    throw LinpipeError{caller, ": A JSON should be an object, but is a '", json.type_name(), "'"};
}

Json json_get_key(const string_view caller, const Json& json, const string_view key) {
  json_assert_object(caller, json);
  if (!json.contains(key))
    throw LinpipeError{caller, ": A JSON was expected to contain a key '", key, "'"};
  return json[key];
}

Json json_get_array(const string_view caller, const Json& json, const string_view key) {
  Json value = json_get_key(caller, json, key);
  if (!value.is_array())
    throw LinpipeError{caller, ": A JSON key '", key, "' should be an array, but is a '", value.type_name(), "'"};
  return value;
}

Json json_get_object(const string_view caller, const Json& json, const string_view key) {
  Json value = json_get_key(caller, json, key);
  if (!value.is_object())
    throw LinpipeError{caller, ": A JSON key '", key, "' should be an object, but is a '", value.type_name(), "'"};
  return value;
}

string json_get_string(const string_view caller, const Json& json, const string_view key) {
  Json value = json_get_key(caller, json, key);
  if (!value.is_string())
    throw LinpipeError{caller, ": A JSON key '", key, "' should be a string, but is a '", value.type_name(), "'"};
  return value;
}

void json_get_string(const string_view caller, const Json& json, const string_view key, string& output) {
  Json value = json_get_key(caller, json, key);
  if (!value.is_string())
    throw LinpipeError{caller, ": A JSON key '", key, "' should be a string, but is a '", value.type_name(), "'"};
  output = value;
}

void json_get_unsigned_vector(const string_view caller, const Json& json, const string_view key, vector<unsigned>& output) {
  Json array = json_get_array(caller, json, key);

  output.clear();
  for (auto& value : array) {
    if (!value.is_number_unsigned())
      throw LinpipeError{caller, ": A JSON key '", key, "' should be an array of unsigned integers, but contains a '", value.type_name(), "'"};
    output.push_back(value);
  }
}

void json_get_unsigned_pair_vector(const string_view caller, const Json& json, const string_view key, vector<pair<unsigned, unsigned>>& output) {
  Json array = json_get_array(caller, json, key);

  output.clear();
  for (auto& value : array) {
    if (!value.is_array())
      throw LinpipeError{caller, ": A JSON key '", key, "' should be an array of unsigned integer pairs, but contains a '", value.type_name(), "'"};
    if (value.size() != 2)
      throw LinpipeError{caller, ": A JSON key '", key, "' should be an array of unsigned integer pairs, but contains an array with ", to_string(value.size()), " items"};
    for (int i = 0; i < 2; i++)
      if (!value[i].is_number_unsigned())
        throw LinpipeError{caller, ": A JSON key '", key, "' should be an array of unsigned integer pairs, but contains a '", value[i].type_name(), "'"};
    output.emplace_back(value[0], value[1]);
  }
}

void json_get_string_vector(const string_view caller, const Json& json, const string_view key, vector<string>& output) {
  Json array = json_get_array(caller, json, key);

  output.clear();
  for (auto& value : array) {
    if (!value.is_string())
      throw LinpipeError{caller, ": A JSON key '", key, "' should be an array of strings, but contains a '", value.type_name(), "'"};
    output.push_back(value);
  }
}

} // namespace linpipe
