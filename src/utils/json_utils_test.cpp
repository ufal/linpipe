// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest/doctest.h"
#include "utils/json_utils.h"

namespace linpipe {

TEST_CASE("json_utils key extraction") {
  Json json_object = {
    {"string", "abc"},
    {"bool", true},
    {"integer", 42},
    {"float", 3.14},
    {"list", {1, 2, 3}},
    {"object", {{"k", "v"}, {"key", "value"}}},
  };

  auto json_string = R"({
    "string": "abc",
    "bool": true,
    "integer": 42,
    "float": 3.14,
    "list": [1, 2, 3],
    "object": {"k": "v", "key": "value"}
  })"sv;

  SUBCASE("parsing json from string") {
    CHECK(json_object == json_parse("json_utils_test", json_string));
  }

  SUBCASE("parsing from a path") {
    CHECK_THROWS_AS(json_parse_path("json_utils_test", "nonexistent.json"), LinpipeError);
  }

  SUBCASE("json_assert_object") {
    CHECK_NOTHROW(json_assert_object("json_utils_test", json_object));
    CHECK_THROWS_AS(json_assert_object("json_utils_test", json_object["list"]), LinpipeError);
  }

  SUBCASE("json_get_key") {
    CHECK(&json_get_key("json_utils_test", json_object, "string") == &json_object["string"]);
    CHECK_THROWS_AS(json_get_key("json_utils_test", json_object, "nonexistent"), LinpipeError);
  }

  SUBCASE("json_get_array") {
    CHECK(&json_get_array("json_utils_test", json_object, "list") == &json_object["list"]);
    CHECK_THROWS_AS(json_get_array("json_utils_test", json_object, "string"), LinpipeError);
  }

  SUBCASE("json_get_object") {
    CHECK(&json_get_object("json_utils_test", json_object, "object") == &json_object["object"]);
    CHECK_THROWS_AS(json_get_object("json_utils_test", json_object, "list"), LinpipeError);
  }
}

// Helpers allowing to test both json_get and json_extract.
struct JsonGet {};
struct JsonExtract {};
template<typename Op, typename T, typename... Args>
T json_get_or_extract(Args&&... args) {
  if constexpr (std::is_same_v<Op, JsonGet>)
    return json_get<T>("json_utils_test", std::forward<Args>(args)...);
  else
    return json_extract<T>("json_utils_test", std::forward<Args>(args)...);
}

TEST_CASE_TEMPLATE("json_utils extracting basic types", Op, JsonGet, JsonExtract) {
  Json json = {
    {"bool", true},
    {"positive_integer", 42},
    {"negative_integer", -42},
    {"float", 3.14},
    {"string", "abc"},
  };

  CHECK(json_get_or_extract<Op, bool>(json, "bool") == true);
  CHECK(json_get_or_extract<Op, int>(json, "positive_integer") == 42);
  CHECK(json_get_or_extract<Op, int>(json, "negative_integer") == -42);
  CHECK(json_get_or_extract<Op, unsigned>(json, "positive_integer") == 42);
  CHECK_THROWS_AS((json_get_or_extract<Op, unsigned>(json, "negative_integer")), LinpipeError);
  CHECK(json_get_or_extract<Op, float>(json, "float") == doctest::Approx(3.14f));
  CHECK(json_get_or_extract<Op, double>(json, "float") == doctest::Approx(3.14));
  CHECK(json_get_or_extract<Op, const std::string&>(json, "string") == "abc");
  CHECK(json_get_or_extract<Op, std::string>(json, "string") == "abc");
  CHECK(json["string"] == (std::is_same_v<Op, JsonGet> ? "abc" : ""));
}

TEST_CASE_TEMPLATE("json_utils extracting containers", Op, JsonGet, JsonExtract) {
  Json json = {
    {"numbers", {1, 2, 3, 4}},
    {"pair", {"a", 1}},
    {"pairs", {{1, "a"}, {2, "b"}, {3, "c"}}},
  };

  SUBCASE("extracting vector") {
    CHECK(json_get_or_extract<Op, std::vector<int>>(json, "numbers") == std::vector<int>{1, 2, 3, 4});
    CHECK_THROWS_AS((json_get_or_extract<Op, std::vector<int>>(json, "pair")), LinpipeError);
  }

  SUBCASE("extracting pair") {
    CHECK(json_get_or_extract<Op, std::pair<std::string, int>>(json, "pair") == std::pair<std::string, int>{"a", 1});
    CHECK(json["pair"][0] == (std::is_same_v<Op, JsonGet> ? "a" : ""));
    CHECK_THROWS_AS((json_get_or_extract<Op, std::pair<int, int>>(json, "pair")), LinpipeError);
    CHECK_THROWS_AS((json_get_or_extract<Op, std::pair<std::string, int>>(json, "numbers")), LinpipeError);
  }

  SUBCASE("extracting tuple") {
    CHECK(json_get_or_extract<Op, std::tuple<int, int, int, int>>(json, "numbers") == std::tuple<int, int, int, int>{1, 2, 3, 4});
    CHECK_THROWS_AS((json_get_or_extract<Op, std::tuple<int, int, int>>(json, "numbers")), LinpipeError);
    CHECK(json_get_or_extract<Op, std::tuple<std::string, int>>(json, "pair") == std::tuple<std::string, int>{"a", 1});
    CHECK(json["pair"][0] == (std::is_same_v<Op, JsonGet> ? "a" : ""));
    CHECK_THROWS_AS((json_get_or_extract<Op, std::tuple<int, int>>(json, "pair")), LinpipeError);
  }

  SUBCASE("extracting vector of pairs") {
    CHECK(json_get_or_extract<Op, std::vector<std::pair<int, std::string>>>(json, "pairs") ==
          std::vector<std::pair<int, std::string>>{{1, "a"}, {2, "b"}, {3, "c"}});
    CHECK(json["pairs"][0][1] == (std::is_same_v<Op, JsonGet> ? "a" : ""));
    CHECK_THROWS_AS((json_get_or_extract<Op, std::vector<std::pair<int, int>>>(json, "pairs")), LinpipeError);
  }

  SUBCASE("extracting vector of tuples") {
    CHECK(json_get_or_extract<Op, std::vector<std::tuple<int, std::string>>>(json, "pairs") ==
          std::vector<std::tuple<int, std::string>>{{1, "a"}, {2, "b"}, {3, "c"}});
    CHECK(json["pairs"][0][1] == (std::is_same_v<Op, JsonGet> ? "a" : ""));
    CHECK_THROWS_AS((json_get_or_extract<Op, std::vector<std::tuple<int, int, std::string>>>(json, "pairs")), LinpipeError);
  }
}

} // namespace linpipe
