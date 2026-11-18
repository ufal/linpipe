// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest/doctest.h"
#include "utils/string_pair_parser.h"

namespace linpipe {

TEST_CASE("parse_string_pairs") {
  using Pairs = std::vector<std::pair<std::string, std::string>>;
  auto parse = [](std::string_view input) { return parse_string_pairs<std::pair<std::string, std::string>>(input); };

  SUBCASE("parses valid inputs") {
    CHECK(parse("") == Pairs{});
    CHECK(parse("value") == Pairs{{"", "value"}});
    CHECK(parse("key=value") == Pairs{{"key", "value"}});
    CHECK(parse("key=") == Pairs{{"key", ""}});
    CHECK(parse("a=b=c") == Pairs{{"a", "b=c"}});
    CHECK(parse("k1=v1:v2:k3=v3") == Pairs{{"k1", "v1"}, {"", "v2"}, {"k3", "v3"}});
    CHECK(parse(":") == Pairs{});
    CHECK(parse("a:") == Pairs{{"", "a"}});
    CHECK(parse(":k=") == Pairs{{"k", ""}});
  }

  SUBCASE("handles escaped colons") {
    CHECK(parse("::") == Pairs{{"", ":"}});
    CHECK(parse("a::b") == Pairs{{"", "a:b"}});
    CHECK(parse("k::1=v::1:x") == Pairs{{"k:1", "v:1"}, {"", "x"}});
    CHECK(parse("a:::b") == Pairs{{"", "a:"}, {"", "b"}});
    CHECK(parse(":::") == Pairs{{"", ":"}});
  }

  SUBCASE("rejects invalid inputs") {
    CHECK_THROWS_AS(parse("=value"), LinPipeError);
    CHECK_THROWS_AS(parse("a:=value"), LinPipeError);
  }
}

} // namespace linpipe
