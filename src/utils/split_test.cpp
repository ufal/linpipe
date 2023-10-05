// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest.h"
#include "utils/split.h"

namespace linpipe {

TEST_CASE("split") {
  vector<string_view> parts;

  SUBCASE("splits empty string") {
    CHECK(split("", ',', parts) == 0); CHECK(parts.empty());
    CHECK(split("", ",", parts) == 0); CHECK(parts.empty());
  }

  SUBCASE("splits one token") {
    CHECK(split("hello", ',', parts) == 1); CHECK(parts == vector{"hello"sv});
    CHECK(split("hello", ",", parts) == 1); CHECK(parts == vector{"hello"sv});
  }

  SUBCASE("splits two tokens") {
    CHECK(split("hello,world", ',', parts) == 2); CHECK(parts == vector{"hello"sv, "world"sv});
    CHECK(split("hello,world", ",", parts) == 2); CHECK(parts == vector{"hello"sv, "world"sv});
    CHECK(split("hello,", ',', parts) == 2); CHECK(parts == vector{"hello"sv, ""sv});
    CHECK(split("hello,", ",", parts) == 2); CHECK(parts == vector{"hello"sv, ""sv});
    CHECK(split(",hello", ',', parts) == 2); CHECK(parts == vector{""sv, "hello"sv});
    CHECK(split(",hello", ",", parts) == 2); CHECK(parts == vector{""sv, "hello"sv});
    CHECK(split("hello, world", ", ", parts) == 2); CHECK(parts == vector{"hello"sv, "world"sv});
  }
}

} // namespace linpipe
