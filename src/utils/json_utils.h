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

#include "common.h"
#include "lib/json/json.h"

namespace linpipe {

// Declarations.
inline Json json_parse(std::string_view input, std::source_location location = std::source_location::current());
inline Json json_parse_path(const std::filesystem::path& path, std::source_location location = std::source_location::current());

inline void json_assert_object(const Json& json, std::source_location location = std::source_location::current());

inline Json& json_get_key(Json& json, std::string_view key, std::source_location location = std::source_location::current());
inline const Json& json_get_key(const Json& json, std::string_view key, std::source_location location = std::source_location::current());

inline Json& json_get_array(Json& json, std::string_view key, std::source_location location = std::source_location::current());
inline const Json& json_get_array(const Json& json, std::string_view key, std::source_location location = std::source_location::current());

inline Json& json_get_object(Json& json, std::string_view key, std::source_location location = std::source_location::current());
inline const Json& json_get_object(const Json& json, std::string_view key, std::source_location location = std::source_location::current());

template<typename T> T json_get(const Json& json, std::source_location location = std::source_location::current());
template<typename T> T json_get(const Json& json, std::string_view key, std::source_location location = std::source_location::current());

template<typename T> T json_extract(Json& json, std::source_location location = std::source_location::current());
template<typename T> T json_extract(Json& json, std::string_view key, std::source_location location = std::source_location::current());

// Definitions.
Json json_parse(std::string_view input, std::source_location location) {
  try {
    return Json::parse(input);
  } catch (const Json::parse_error& error) {
    throw LinpipeError(location, "Cannot parse JSON: {}", error.what());
  }
}

Json json_parse_path(const std::filesystem::path& path, std::source_location location) {
  std::ifstream is(path);
  if (!is.is_open())
    throw LinpipeError(location, "Cannot open file '{}'", path);
  try {
    return Json::parse(is);
  } catch (const Json::parse_error& error) {
    throw LinpipeError(location, "Cannot parse JSON from file '{}': {}", path, error.what());
  }
}

void json_assert_object(const Json& json, std::source_location location) {
  if (!json.is_object())
    throw LinpipeError(location, "A JSON should be an object, but is a '{}'", json.type_name());
}

const Json& json_get_key(const Json& json, std::string_view key, std::source_location location) {
  json_assert_object(json, location);
  auto it = json.find(key);
  if (it == json.end())
    throw LinpipeError(location, "A JSON was expected to contain a key '{}'", key);
  return *it;
}

Json& json_get_key(Json& json, std::string_view key, std::source_location location) {
  return const_cast<Json&>(json_get_key(std::as_const(json), key, location));
}

const Json& json_get_array(const Json& json, std::string_view key, std::source_location location) {
  const auto& value = json_get_key(json, key, location);
  if (!value.is_array())
    throw LinpipeError(location, "A JSON value at '{}' should be an array, but is a '{}'", key, value.type_name());
  return value;
}

Json& json_get_array(Json& json, std::string_view key, std::source_location location) {
  return const_cast<Json&>(json_get_array(std::as_const(json), key, location));
}

const Json& json_get_object(const Json& json, std::string_view key, std::source_location location) {
  const auto& value = json_get_key(json, key, location);
  if (!value.is_object())
    throw LinpipeError(location, "A JSON value at '{}' should be an object, but is a '{}'", key, value.type_name());
  return value;
}

Json& json_get_object(Json& json, std::string_view key, std::source_location location) {
  return const_cast<Json&>(json_get_object(std::as_const(json), key, location));
}

// A helper struct allowing partial specialization handling of json_get and json_extract.
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
T json_get(const Json& json, std::source_location location) {
  return JsonExtractor<T>::convert(json, JsonExtractorPath{nullptr, std::string_view()}, location);
}

template<typename T>
T json_get(const Json& json, std::string_view key, std::source_location location) {
  return JsonExtractor<T>::convert(json_get_key(json, key, location), JsonExtractorPath{nullptr, key}, location);
}

template<typename T>
T json_extract(Json& json, std::source_location location) {
  return JsonExtractor<T>::convert(json, JsonExtractorPath{nullptr, std::string_view()}, location);
}

template<typename T>
T json_extract(Json& json, std::string_view key, std::source_location location) {
  return JsonExtractor<T>::convert(json_get_key(json, key, location), JsonExtractorPath{nullptr, key}, location);
}

template<>
struct JsonExtractor<bool> {
  static bool convert(const Json& json, const JsonExtractorPath& path, std::source_location location) {
    if (!json.is_boolean())
      throw LinpipeError(location, "A JSON value at '{}' should be a boolean, but is a '{}'", path.str(), json.type_name());
    return json.get<bool>();
  }
};

template<std::integral T>
  requires (!std::same_as<T, bool>)
struct JsonExtractor<T> {
  static T convert(const Json& json, const JsonExtractorPath& path, std::source_location location) {
    if (json.is_number_unsigned()) {
      if (auto value = json.get<std::uint64_t>(); std::in_range<T>(value))
        return static_cast<T>(value);
    } else if (json.is_number_integer()) {
      if (auto value = json.get<std::int64_t>(); std::in_range<T>(value))
        return static_cast<T>(value);
    } else {
      throw LinpipeError(location, "A JSON value at '{}' should be an{} integer, but is a '{}'",
                         path.str(), std::is_signed_v<T> ? "" : " unsigned", json.type_name());
    }
    throw LinpipeError(location, "A JSON value at '{}' contains an integer out of range", path.str());
  }
};

template<std::floating_point T>
struct JsonExtractor<T> {
  static T convert(const Json& json, const JsonExtractorPath& path, std::source_location location) {
    if (!json.is_number())
      throw LinpipeError(location, "A JSON value at '{}' should be a number, but is a '{}'", path.str(), json.type_name());
    return json.get<T>();
  }
};

template<>
struct JsonExtractor<const std::string&> {
  static const std::string& convert(const Json& json, const JsonExtractorPath& path, std::source_location location) {
    if (!json.is_string())
      throw LinpipeError(location, "A JSON value at '{}' should be a string, but is a '{}'", path.str(), json.type_name());
    return json.get_ref<const std::string&>();
  }
};

template<>
struct JsonExtractor<std::string> {
  template<typename J>
  static std::string convert(J& json, const JsonExtractorPath& path, std::source_location location) {
    if (!json.is_string())
      throw LinpipeError(location, "A JSON value at '{}' should be a string, but is a '{}'", path.str(), json.type_name());
    if constexpr (std::is_const_v<J>)
      return json.template get<std::string>();
    else
      return std::move(json.template get_ref<std::string&>());
  }
};

template<typename T>
struct JsonExtractor<std::vector<T>> {
  template<typename J>
  static std::vector<T> convert(J& json, const JsonExtractorPath& path, std::source_location location) {
    if (!json.is_array())
      throw LinpipeError(location, "A JSON value at '{}' should be an array, but is a '{}'", path.str(), json.type_name());
    std::vector<T> output;
    output.reserve(json.size());
    size_t index = 0;
    for (auto& item : json)
      output.push_back(JsonExtractor<T>::convert(item, JsonExtractorPath{&path, index++}, location));
    return output;
  }
};

template<typename T, typename U>
struct JsonExtractor<std::pair<T, U>> {
  template<typename J>
  static std::pair<T, U> convert(J& json, const JsonExtractorPath& path, std::source_location location) {
    if (!json.is_array())
      throw LinpipeError(location, "A JSON value at '{}' should be a pair as an array, but is a '{}'", path.str(), json.type_name());
    if (json.size() != 2)
      throw LinpipeError(location, "A JSON value at '{}' should be an array of size 2, but has size {}", path.str(), json.size());
    return {
      JsonExtractor<T>::convert(json[0], JsonExtractorPath{&path, 0}, location),
      JsonExtractor<U>::convert(json[1], JsonExtractorPath{&path, 1}, location)};
  }
};

template<typename... Ts>
struct JsonExtractor<std::tuple<Ts...>> {
  template<typename J>
  static std::tuple<Ts...> convert(J& json, const JsonExtractorPath& path, std::source_location location) {
    if (!json.is_array())
      throw LinpipeError(location, "A JSON value at '{}' should be a tuple as an array, but is a '{}'", path.str(), json.type_name());
    if (json.size() != sizeof...(Ts))
      throw LinpipeError(location, "A JSON value at '{}' should be an array of size {}, but has size {}",
                         path.str(), sizeof...(Ts), json.size());
    return convert_(json, path, std::index_sequence_for<Ts...>{}, location);
  }

 private:
  // A static helper instead of a template lambda to side-step a bug of older MSVC.
  template<typename J, size_t... Is>
  static std::tuple<Ts...> convert_(J& json, const JsonExtractorPath& path, std::index_sequence<Is...> /*is*/, std::source_location location) {
    return std::tuple<Ts...>{JsonExtractor<Ts>::convert(json[Is], JsonExtractorPath{&path, Is}, location)...};
  }
};

} // namespace linpipe
