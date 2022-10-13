// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "lib/doctest.h"
#include "lib/json.h"

namespace linpipe {

TEST_CASE("json") {
  Json j = {
    {"string", "abc"},
    {"bool", true},
    {"integer", 42},
    {"float", 3.14},
    {"list", {1, 2, 3}},
    {"object", {
      {"k", "v"},
      {"key", "value"},
    }},
  };

  SUBCASE("parsing json from string") {
    REQUIRE(j == Json::parse(R"(
      {
        "string": "abc",
        "bool": true,
        "integer": 42,
        "float": 3.14,
        "list": [1, 2, 3],
        "object": {
          "k": "v",
          "key": "value"
        }
      }
    )"));
  }

  SUBCASE("json string literals") {
    REQUIRE(j == R"(
      {
        "string": "abc",
        "bool": true,
        "integer": 42,
        "float": 3.14,
        "list": [1, 2, 3],
        "object": {
          "k": "v",
          "key": "value"
        }
      }
    )"_json);
  }
}

} // namespace linpipe
