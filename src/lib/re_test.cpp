// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest/doctest.h"
#include "lib/re.h"

namespace linpipe {

// clang-format off
TEST_CASE("RE::RE") {
  CHECK_NOTHROW(RE("^$"));
  CHECK_THROWS_AS(RE("("), LinpipeError);
}

TEST_CASE("RE::match") {
  RE::Match match;

  SUBCASE("no groups") {
    CHECK(RE("\\w").match("Hi"));
    CHECK(RE("\\w").match("Hi", &match)); CHECK(match == "H"sv);
    CHECK(!RE("\\w").match("@"));
    CHECK(!RE("\\w").match("@", &match)); CHECK(!match);
    CHECK(RE("\\w").match("\xc4\x8d", &match)); CHECK(match == "\xc4\x8d"sv);
    CHECK(RE("\\p{Ll}").match("\xc4\x8d", &match)); CHECK(match == "\xc4\x8d"sv);
    CHECK(!RE("\\p{Ll}").match("\xc4\x8c"));
  }
  SUBCASE("groups") {
    CHECK(RE(".(\\w).(\\w)").match("abcde", &match));
    CHECK(match.groups == std::vector{"b"sv, "d"sv});
  }
}

TEST_CASE("RE::search") {
  RE::Match match;

  SUBCASE("no groups") {
    CHECK(RE("\\w+").search("Hi"));
    CHECK(RE("\\w+").search("Hi", &match)); CHECK(match == "Hi"sv);
    CHECK(RE("\\w+").search("@Hi", &match)); CHECK(match == "Hi"sv);
    CHECK(RE("\\p{Ll}").search("\xc4\x8c\xc4\x8d", &match)); CHECK(match == "\xc4\x8d"sv);
  }
  SUBCASE("groups") {
    CHECK(RE("(\\w)!(\\w)").search("Hi\xc4\x8c!\xc4\x8dthere", &match));
    CHECK(match == "\xc4\x8c!\xc4\x8d"sv);
    CHECK(match.groups == std::vector{"\xc4\x8c"sv, "\xc4\x8d"sv});
  }
}

TEST_CASE("RE::split") {
  std::vector<std::string_view> parts;

  CHECK(RE(",").split("", parts) == 0); CHECK(parts.empty());

  CHECK(RE(",").split("a", parts) == 1); CHECK(parts == std::vector{"a"sv});

  CHECK(RE(",").split("a,b,c", parts) == 3); CHECK(parts == std::vector{"a"sv, "b"sv, "c"sv});

  CHECK(RE(",").split("a,b,c", parts, 1) == 2); CHECK(parts == std::vector{"a"sv, "b,c"sv});

  CHECK(RE(",").split(",a,b,c,", parts) == 5); CHECK(parts == std::vector{""sv, "a"sv, "b"sv, "c"sv, ""sv});

  CHECK(RE("\\d+").split("a42b1c1234", parts) == 4); CHECK(parts == std::vector{"a"sv, "b"sv, "c"sv, ""sv});

  CHECK(RE("(?<=[ab])\\d(?=b)").split("a1a2a3b4b6", parts) == 3); CHECK(parts == std::vector{"a1a2a"sv, "b"sv, "b6"sv});

  CHECK(RE("(?=\\d)").split("a1b1c1d", parts) == 4); CHECK(parts == std::vector{"a"sv, "1b"sv, "1c"sv, "1d"sv});
  CHECK(RE("(?=\\d)").split("1b1c1d", parts) == 4); CHECK(parts == std::vector{""sv, "1b"sv, "1c"sv, "1d"sv});
  CHECK(RE("(?<=\\d)").split("a1b1c1d", parts) == 4); CHECK(parts == std::vector{"a1"sv, "b1"sv, "c1"sv, "d"sv});
  CHECK(RE("(?<=\\d)").split("a1b1c1", parts) == 4); CHECK(parts == std::vector{"a1"sv, "b1"sv, "c1"sv, ""sv});
}

TEST_CASE("RE::sub") {
  std::string result;

  CHECK(RE("a").sub("abacad", "_", result) == 3); CHECK(result == "_b_c_d");
  CHECK(RE("a").sub("abacad", "_", result, 1) == 1); CHECK(result == "_bacad");
  CHECK(RE("a").sub("abaca", "_", result) == 3); CHECK(result == "_b_c_");
  CHECK(RE("a").sub("baca", "_", result) == 2); CHECK(result == "b_c_");

  CHECK(RE("(\\d+)").sub("a1b22c333d", "_", result) == 3); CHECK(result == "a_b_c_d");
  CHECK(RE("(\\d)").sub("a1b2c3d", "\\1\\1", result) == 3); CHECK(result == "a11b22c33d");
  CHECK(RE("(\\d)").sub("a1b2c3d", "\\1\\2", result) == 3); CHECK(result == "a1\\2b2\\2c3\\2d");
  CHECK(RE("(\\d)").sub("a1b2c3d", "\\1\\\\", result) == 3); CHECK(result == "a1\\b2\\c3\\d");

  CHECK(RE("").sub("bcd", "_", result) == 4); CHECK(result == "_b_c_d_");
}

TEST_CASE("RE::options") {
  RE::Match match;

  CHECK(RE("A+").search("aA", &match)); CHECK(match == "A"sv);
  CHECK(RE("A+", RE::IGNORECASE).search("aA", &match)); CHECK(match == "aA"sv);

  CHECK(RE("A.*B").search("AB\nAB", &match)); CHECK(match == "AB"sv);
  CHECK(RE("A.*B", RE::DOTALL).search("AB\nAB", &match)); CHECK(match == "AB\nAB"sv);

  CHECK(!RE("^B").search("A\nB"));
  CHECK(RE("^B", RE::MULTILINE).search("A\nB", &match)); CHECK(match == "B"sv);
}

TEST_CASE("RE32::RE32") {
  CHECK_NOTHROW(RE32("^$"));
  CHECK_NOTHROW(RE32(U"^$"));
  CHECK_THROWS_AS(RE32("("), LinpipeError);
  CHECK_THROWS_AS(RE32(U"("), LinpipeError);
}

TEST_CASE("RE32::match") {
  RE32::Match match;

  SUBCASE("no groups") {
    CHECK(RE32("\\w").match(U"Hi"));
    CHECK(RE32("\\w").match(U"Hi", &match)); CHECK(match == U"H"sv);
    CHECK(!RE32("\\w").match(U"@"));
    CHECK(!RE32("\\w").match(U"@", &match)); CHECK(!match);
    CHECK(RE32("\\w").match(U"\u010d", &match)); CHECK(match == U"\u010d"sv);
    CHECK(RE32("\\p{Ll}").match(U"\u010d", &match)); CHECK(match == U"\u010d"sv);
    CHECK(!RE32("\\p{Ll}").match(U"\u010c"));
    CHECK(RE32("\xc4\x8c").match(U"\u010c", &match)); CHECK(match == U"\u010c"sv);
    CHECK(RE32("[\xc4\x8c\xc4\x8d]").match(U"\u010d", &match)); CHECK(match == U"\u010d"sv);
    CHECK(RE32(U"[\u010c\u010d]").match(U"\u010d", &match)); CHECK(match == U"\u010d"sv);
  }
  SUBCASE("groups") {
    CHECK(RE32(".(\\w).(\\w)").match(U"abcde", &match));
    CHECK(match.groups == std::vector{U"b"sv, U"d"sv});
  }
}

TEST_CASE("RE32::search") {
  RE32::Match match;

  SUBCASE("no groups") {
    CHECK(RE32("\\w+").search(U"Hi"));
    CHECK(RE32("\\w+").search(U"Hi", &match)); CHECK(match == U"Hi"sv);
    CHECK(RE32("\\w+").search(U"@Hi", &match)); CHECK(match == U"Hi"sv);
    CHECK(RE32("\\p{Ll}").search(U"\u010c\u010d", &match)); CHECK(match == U"\u010d"sv);
  }
  SUBCASE("groups") {
    CHECK(RE32("(\\w)!(\\w)").search(U"Hi\u010c!\u010dthere", &match));
    CHECK(match == U"\u010c!\u010d"sv);
    CHECK(match.groups == std::vector{U"\u010c"sv, U"\u010d"sv});
  }
}

TEST_CASE("RE32::split") {
  std::vector<std::u32string_view> parts;

  CHECK(RE32(",").split(U"", parts) == 0); CHECK(parts.empty());

  CHECK(RE32(",").split(U"a", parts) == 1); CHECK(parts == std::vector{U"a"sv});

  CHECK(RE32(",").split(U"a,b,c", parts) == 3); CHECK(parts == std::vector{U"a"sv, U"b"sv, U"c"sv});

  CHECK(RE32(",").split(U"a,b,c", parts, 1) == 2); CHECK(parts == std::vector{U"a"sv, U"b,c"sv});

  CHECK(RE32(",").split(U",a,b,c,", parts) == 5); CHECK(parts == std::vector{U""sv, U"a"sv, U"b"sv, U"c"sv, U""sv});

  CHECK(RE32("\\d+").split(U"a42b1c1234", parts) == 4); CHECK(parts == std::vector{U"a"sv, U"b"sv, U"c"sv, U""sv});

  CHECK(RE32("(?<=[ab])\\d(?=b)").split(U"a1a2a3b4b6", parts) == 3); CHECK(parts == std::vector{U"a1a2a"sv, U"b"sv, U"b6"sv});

  CHECK(RE32("(?=\\d)").split(U"a1b1c1d", parts) == 4); CHECK(parts == std::vector{U"a"sv, U"1b"sv, U"1c"sv, U"1d"sv});
  CHECK(RE32("(?=\\d)").split(U"1b1c1d", parts) == 4); CHECK(parts == std::vector{U""sv, U"1b"sv, U"1c"sv, U"1d"sv});
  CHECK(RE32("(?<=\\d)").split(U"a1b1c1d", parts) == 4); CHECK(parts == std::vector{U"a1"sv, U"b1"sv, U"c1"sv, U"d"sv});
  CHECK(RE32("(?<=\\d)").split(U"a1b1c1", parts) == 4); CHECK(parts == std::vector{U"a1"sv, U"b1"sv, U"c1"sv, U""sv});
}

TEST_CASE("RE32::sub") {
  std::u32string result;

  CHECK(RE32("a").sub(U"abacad", U"_", result) == 3); CHECK(result == U"_b_c_d");
  CHECK(RE32("a").sub(U"abacad", U"_", result, 1) == 1); CHECK(result == U"_bacad");
  CHECK(RE32("a").sub(U"abaca", U"_", result) == 3); CHECK(result == U"_b_c_");
  CHECK(RE32("a").sub(U"baca", U"_", result) == 2); CHECK(result == U"b_c_");

  CHECK(RE32("(\\d+)").sub(U"a1b22c333d", U"_", result) == 3); CHECK(result == U"a_b_c_d");
  CHECK(RE32("(\\d)").sub(U"a1b2c3d", U"\\1\\1", result) == 3); CHECK(result == U"a11b22c33d");
  CHECK(RE32("(\\d)").sub(U"a1b2c3d", U"\\1\\2", result) == 3); CHECK(result == U"a1\\2b2\\2c3\\2d");
  CHECK(RE32("(\\d)").sub(U"a1b2c3d", U"\\1\\\\", result) == 3); CHECK(result == U"a1\\b2\\c3\\d");

  CHECK(RE32("").sub(U"bcd", U"_", result) == 4); CHECK(result == U"_b_c_d_");
}

TEST_CASE("RE32::options") {
  RE32::Match match;

  CHECK(RE32("A+").search(U"aA", &match)); CHECK(match == U"A"sv);
  CHECK(RE32("A+", RE32::IGNORECASE).search(U"aA", &match)); CHECK(match == U"aA"sv);

  CHECK(RE32("A.*B").search(U"AB\nAB", &match)); CHECK(match == U"AB"sv);
  CHECK(RE32("A.*B", RE32::DOTALL).search(U"AB\nAB", &match)); CHECK(match == U"AB\nAB"sv);

  CHECK(!RE32("^B").search(U"A\nB"));
  CHECK(RE32("^B", RE32::MULTILINE).search(U"A\nB", &match)); CHECK(match == U"B"sv);
}

} // namespace linpipe
