// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <fstream>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include "common.h"
#include "lib/json/json.h"

namespace linpipe {

// Declarations.
inline Json json_parse(std::string_view caller, std::string_view input);
inline Json json_parse_path(std::string_view caller, const std::filesystem::path& path);

inline void json_assert_object(std::string_view caller, const Json& json);

inline Json& json_get_key(std::string_view caller, Json& json, std::string_view key);
inline const Json& json_get_key(std::string_view caller, const Json& json, std::string_view key);

inline Json& json_get_array(std::string_view caller, Json& json, std::string_view key);
inline const Json& json_get_array(std::string_view caller, const Json& json, std::string_view key);

inline Json& json_get_object(std::string_view caller, Json& json, std::string_view key);
inline const Json& json_get_object(std::string_view caller, const Json& json, std::string_view key);

template<typename T>
T json_extract_key(std::string_view caller, Json& json, std::string_view key);

// Definitions.
Json json_parse(std::string_view caller, std::string_view input) {
  try {
    return Json::parse(input);
  } catch (const Json::parse_error& error) {
    throw LinpipeError{caller, ": Cannot parse JSON: ", error.what()};
  }
}

Json json_parse_path(std::string_view caller, const std::filesystem::path& path) {
  std::ifstream is(path);
  if (!is.is_open())
    throw LinpipeError{caller, ": Cannot open file '", path.string(), "'"};
  try {
    return Json::parse(is);
  } catch (const Json::parse_error& error) {
    throw LinpipeError{caller, ": Cannot parse JSON from file '", path.string(), "': ", error.what()};
  }
}

void json_assert_object(std::string_view caller, const Json& json) {
  if (!json.is_object())
    throw LinpipeError{caller, ": A JSON should be an object, but is a '", json.type_name(), "'"};
}

const Json& json_get_key(std::string_view caller, const Json& json, std::string_view key) {
  json_assert_object(caller, json);
  auto it = json.find(key);
  if (it == json.end())
    throw LinpipeError{caller, ": A JSON was expected to contain a key '", key, "'"};
  return *it;
}

Json& json_get_key(std::string_view caller, Json& json, std::string_view key) {
  return const_cast<Json&>(json_get_key(caller, std::as_const(json), key));
}

const Json& json_get_array(std::string_view caller, const Json& json, std::string_view key) {
  const auto& value = json_get_key(caller, json, key);
  if (!value.is_array())
    throw LinpipeError{caller, ": A JSON value at '", key, "' should be an array, but is a '", value.type_name(), "'"};
  return value;
}

Json& json_get_array(std::string_view caller, Json& json, std::string_view key) {
  return const_cast<Json&>(json_get_array(caller, std::as_const(json), key));
}

const Json& json_get_object(std::string_view caller, const Json& json, std::string_view key) {
  const auto& value = json_get_key(caller, json, key);
  if (!value.is_object())
    throw LinpipeError{caller, ": A JSON value at '", key, "' should be an object, but is a '", value.type_name(), "'"};
  return value;
}

Json& json_get_object(std::string_view caller, Json& json, std::string_view key) {
  return const_cast<Json&>(json_get_object(caller, std::as_const(json), key));
}

// A helper struct allowing partial specialization handling of json_extract_key for container types.
template<typename T> struct JsonExtractor;

// A helper class for efficient reporting of JSON errors.
class JsonExtractorPath {
 public:
  JsonExtractorPath(const JsonExtractorPath* parent, std::string_view key) : parent_(parent), key_(key) {}
  JsonExtractorPath(const JsonExtractorPath* parent, size_t index) : parent_(parent), index_(index) {}

  std::string str() const {
    std::string result = parent_ ? parent_->str() : std::string{};
    if (index_ != no_index)
      result.append("[").append(std::to_string(index_)).append("]");
    else
      result.append(result.empty() ? "" : ".").append(key_);
    return result;
  }

 private:
  static constexpr size_t no_index = std::numeric_limits<size_t>::max();
  const JsonExtractorPath* parent_;
  std::string_view key_;
  size_t index_ = no_index;
};

template<typename T>
T json_extract_key(std::string_view caller, Json& json, std::string_view key) {
  return JsonExtractor<T>::extract(caller, json_get_key(caller, json, key), JsonExtractorPath{nullptr, key});
}

template<>
struct JsonExtractor<bool> {
  static bool extract(std::string_view caller, Json& json, const JsonExtractorPath& path) {
    if (!json.is_boolean())
      throw LinpipeError{caller, ": A JSON value at '", path.str(), "' should be a boolean, but is a '", json.type_name(), "'"};
    return json.get<bool>();
  }
};

template<std::integral T>
  requires (!std::same_as<T, bool>)
struct JsonExtractor<T> {
  static T extract(std::string_view caller, Json& json, const JsonExtractorPath& path) {
    if (json.is_number_unsigned()) {
      if (auto value = json.get<std::uint64_t>(); std::in_range<T>(value))
        return static_cast<T>(value);
    } else if (json.is_number_integer()) {
      if (auto value = json.get<std::int64_t>(); std::in_range<T>(value))
        return static_cast<T>(value);
    } else {
      throw LinpipeError{caller, ": A JSON value at '", path.str(), "' should be an",
                         std::is_signed_v<T> ? "" : " unsigned", " integer, but is a '", json.type_name(), "'"};
    }
    throw LinpipeError{caller, ": A JSON value at '", path.str(), "' contains an integer out of range"};
  }
};

template<std::floating_point T>
struct JsonExtractor<T> {
  static T extract(std::string_view caller, Json& json, const JsonExtractorPath& path) {
    if (!json.is_number())
      throw LinpipeError{caller, ": A JSON value at '", path.str(), "' should be a number, but is a '", json.type_name(), "'"};
    return json.get<T>();
  }
};

template<>
struct JsonExtractor<std::string> {
  static std::string extract(std::string_view caller, Json& json, const JsonExtractorPath& path) {
    if (!json.is_string())
      throw LinpipeError{caller, ": A JSON value at '", path.str(), "' should be a string, but is a '", json.type_name(), "'"};
    return std::move(json.get_ref<std::string&>());
  }
};

template<typename T>
struct JsonExtractor<std::vector<T>> {
  static std::vector<T> extract(std::string_view caller, Json& json, const JsonExtractorPath& path) {
    if (!json.is_array())
      throw LinpipeError{caller, ": A JSON value at '", path.str(), "' should be an array, but is a '", json.type_name(), "'"};
    std::vector<T> output;
    output.reserve(json.size());
    size_t index = 0;
    for (auto& item : json)
      output.push_back(JsonExtractor<T>::extract(caller, item, JsonExtractorPath{&path, index++}));
    return output;
  }
};

template<typename T, typename U>
struct JsonExtractor<std::pair<T, U>> {
  static std::pair<T, U> extract(std::string_view caller, Json& json, const JsonExtractorPath& path) {
    if (!json.is_array())
      throw LinpipeError{caller, ": A JSON value at '", path.str(), "' should be a pair as an array, but is a '", json.type_name(), "'"};
    if (json.size() != 2)
      throw LinpipeError{caller, ": A JSON value at '", path.str(), "' should be an array of size 2, but has size ", std::to_string(json.size())};
    return {
      JsonExtractor<T>::extract(caller, json[0], JsonExtractorPath{&path, 0}),
      JsonExtractor<U>::extract(caller, json[1], JsonExtractorPath{&path, 1})};
  }
};

template<typename... Ts>
struct JsonExtractor<std::tuple<Ts...>> {
  static std::tuple<Ts...> extract(std::string_view caller, Json& json, const JsonExtractorPath& path) {
    if (!json.is_array())
      throw LinpipeError{caller, ": A JSON value at '", path.str(), "' should be a tuple as an array, but is a '", json.type_name(), "'"};
    if (json.size() != sizeof...(Ts))
      throw LinpipeError{caller, ": A JSON value at '", path.str(), "' should be an array of size ",
                         std::to_string(sizeof...(Ts)), ", but has size ", std::to_string(json.size())};
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
      return std::tuple<Ts...>{JsonExtractor<Ts>::extract(caller, json[Is], JsonExtractorPath{&path, Is})...};
    }(std::index_sequence_for<Ts...>{});
  }
};

} // namespace linpipe
