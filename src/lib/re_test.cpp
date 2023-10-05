// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest.h"
#include "lib/re.h"

namespace linpipe {

TEST_CASE("RE::RE") {
  CHECK_NOTHROW(RE("^$"));
  CHECK_THROWS_AS(RE("("), LinpipeError);
}

TEST_CASE("RE::match") {
  SUBCASE("no groups") {
    CHECK(RE("\\w").match("Hi") == "H");
    CHECK(RE("\\w").match("@").data() == nullptr);
    CHECK(RE("\\w").match("\xc4\x8d") == "\xc4\x8d");
    CHECK(RE("\\p{Ll}").match("\xc4\x8d") == "\xc4\x8d");
    CHECK(RE("\\p{Ll}").match("\xc4\x8c").data() == nullptr);
  }
  SUBCASE("groups") {
    vector<string_view> groups;
    CHECK(RE(".(\\w).(\\w)").match("abcde", &groups) == "abcd"); CHECK(groups == vector{"b"sv, "d"sv});
  }
}

TEST_CASE("RE::search") {
  SUBCASE("no groups") {
    CHECK(RE("\\w+").search("Hi") == "Hi");
    CHECK(RE("\\w+").search("@Hi") == "Hi");
    CHECK(RE("\\p{Ll}").search("\xc4\x8c\xc4\x8d") == "\xc4\x8d");
  }
  SUBCASE("groups") {
    vector<string_view> groups;
    CHECK(RE("(\\w)!(\\w)").search("Hi\xc4\x8c!\xc4\x8dthere", &groups) == "\xc4\x8c!\xc4\x8d");
    CHECK(groups == vector{"\xc4\x8c"sv, "\xc4\x8d"sv});
  }
}

TEST_CASE("RE::split") {
  vector<string_view> parts;
  CHECK(RE(",").split("", parts) == 0); CHECK(parts.empty());

  CHECK(RE(",").split("a", parts) == 1); CHECK(parts == vector{"a"sv});

  CHECK(RE(",").split("a,b,c", parts) == 3); CHECK(parts == vector{"a"sv, "b"sv, "c"sv});

  CHECK(RE(",").split("a,b,c", parts, 1) == 2); CHECK(parts == vector{"a"sv, "b,c"sv});

  CHECK(RE(",").split(",a,b,c,", parts) == 5); CHECK(parts == vector{""sv, "a"sv, "b"sv, "c"sv, ""sv});

  CHECK(RE("\\d+").split("a42b1c1234", parts) == 4); CHECK(parts == vector{"a"sv, "b"sv, "c"sv, ""sv});

  CHECK(RE("(?<=[ab])\\d(?=b)").split("a1a2a3b4b6", parts) == 3); CHECK(parts == vector{"a1a2a"sv, "b"sv, "b6"sv});

  CHECK(RE("(?=\\d)").split("a1b1c1d", parts) == 4); CHECK(parts == vector{"a"sv, "1b"sv, "1c"sv, "1d"sv});
  CHECK(RE("(?=\\d)").split("1b1c1d", parts) == 4); CHECK(parts == vector{""sv, "1b"sv, "1c"sv, "1d"sv});
  CHECK(RE("(?<=\\d)").split("a1b1c1d", parts) == 4); CHECK(parts == vector{"a1"sv, "b1"sv, "c1"sv, "d"sv});
  CHECK(RE("(?<=\\d)").split("a1b1c1", parts) == 4); CHECK(parts == vector{"a1"sv, "b1"sv, "c1"sv, ""sv});
}

TEST_CASE("RE::sub") {
  string result;
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

TEST_CASE("RE32::RE32") {
  CHECK_NOTHROW(RE32("^$"));
  CHECK_NOTHROW(RE32(U"^$"));
  CHECK_THROWS_AS(RE32("("), LinpipeError);
  CHECK_THROWS_AS(RE32(U"("), LinpipeError);
}

TEST_CASE("RE32::match") {
  SUBCASE("no groups") {
    CHECK(RE32("\\w").match(U"Hi") == U"H");
    CHECK(RE32("\\w").match(U"@").data() == nullptr);
    CHECK(RE32("\\w").match(U"\u010d") == U"\u010d");
    CHECK(RE32("\\p{Ll}").match(U"\u010d") == U"\u010d");
    CHECK(RE32("\\p{Ll}").match(U"\u010c").data() == nullptr);
    CHECK(RE32("\xc4\x8c").match(U"\u010c").data() == U"\u010c");
    CHECK(RE32("[\xc4\x8c\xc4\x8d]").match(U"\u010d").data() == U"\u010d");
    CHECK(RE32(U"[\u010c\u010d]").match(U"\u010d").data() == U"\u010d");
  }
  SUBCASE("groups") {
    vector<u32string_view> groups;
    CHECK(RE32(".(\\w).(\\w)").match(U"abcde", &groups) == U"abcd"); CHECK(groups == vector{U"b"sv, U"d"sv});
  }
}

TEST_CASE("RE32::search") {
  SUBCASE("no groups") {
    CHECK(RE32("\\w+").search(U"Hi") == U"Hi");
    CHECK(RE32("\\w+").search(U"@Hi") == U"Hi");
    CHECK(RE32("\\p{Ll}").search(U"\u010c\u010d") == U"\u010d");
  }
  SUBCASE("groups") {
    vector<u32string_view> groups;
    CHECK(RE32("(\\w)!(\\w)").search(U"Hi\u010c!\u010dthere", &groups) == U"\u010c!\u010d");
    CHECK(groups == vector{U"\u010c"sv, U"\u010d"sv});
  }
}

TEST_CASE("RE32::split") {
  vector<u32string_view> parts;
  CHECK(RE32(",").split(U"", parts) == 0); CHECK(parts.empty());

  CHECK(RE32(",").split(U"a", parts) == 1); CHECK(parts == vector{U"a"sv});

  CHECK(RE32(",").split(U"a,b,c", parts) == 3); CHECK(parts == vector{U"a"sv, U"b"sv, U"c"sv});

  CHECK(RE32(",").split(U"a,b,c", parts, 1) == 2); CHECK(parts == vector{U"a"sv, U"b,c"sv});

  CHECK(RE32(",").split(U",a,b,c,", parts) == 5); CHECK(parts == vector{U""sv, U"a"sv, U"b"sv, U"c"sv, U""sv});

  CHECK(RE32("\\d+").split(U"a42b1c1234", parts) == 4); CHECK(parts == vector{U"a"sv, U"b"sv, U"c"sv, U""sv});

  CHECK(RE32("(?<=[ab])\\d(?=b)").split(U"a1a2a3b4b6", parts) == 3); CHECK(parts == vector{U"a1a2a"sv, U"b"sv, U"b6"sv});

  CHECK(RE32("(?=\\d)").split(U"a1b1c1d", parts) == 4); CHECK(parts == vector{U"a"sv, U"1b"sv, U"1c"sv, U"1d"sv});
}

TEST_CASE("RE32::sub") {
  u32string result;
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

} // namespace linpipe
