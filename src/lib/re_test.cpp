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
    CHECK(RE("\\w").match("@") == string_view());
    CHECK(RE("\\w").match("\xc4\x8d") == "\xc4\x8d");
    CHECK(RE("\\p{Ll}").match("\xc4\x8d") == "\xc4\x8d");
    CHECK(RE("\\p{Ll}").match("\xc4\x8c") == string_view());
  }
  SUBCASE("groups") {
    vector<string_view> groups;
    CHECK(RE(".(\\w).(\\w)").match("abcde", &groups) == "abcd");
    CHECK(groups.size() == 2); CHECK(groups[0] == "b"); CHECK(groups[1] == "d");
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
    CHECK(groups[0] == "\xc4\x8c");
    CHECK(groups[1] == "\xc4\x8d");
  }
}

TEST_CASE("RE::split") {
  vector<string_view> parts;
  CHECK(RE(",").split("a,b,c", parts) == 3);
  CHECK(parts.size() == 3); CHECK(parts[0] == "a"); CHECK(parts[1] == "b"); CHECK(parts[2] == "c");

  CHECK(RE(",").split("a,b,c", parts, 1) == 2);
  CHECK(parts.size() == 2); CHECK(parts[0] == "a"); CHECK(parts[1] == "b,c");

  CHECK(RE(",").split(",a,b,c,", parts) == 5);
  CHECK(parts.size() == 5); CHECK(parts[0] == ""); CHECK(parts[1] == "a");
  CHECK(parts[2] == "b"); CHECK(parts[3] == "c"); CHECK(parts[4] == "");

  CHECK(RE("\\d+").split("a42b1c1234", parts) == 4);
  CHECK(parts.size() == 4); CHECK(parts[0] == "a"); CHECK(parts[1] == "b"); CHECK(parts[2] == "c"); CHECK(parts[3] == "");

  CHECK(RE("(?<=[ab])\\d(?=b)").split("a1a2a3b4b6", parts) == 3);
  CHECK(parts.size() == 3); CHECK(parts[0] == "a1a2a"); CHECK(parts[1] == "b"); CHECK(parts[2] == "b6");
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
    CHECK(RE32("\\w").match(U"@") == u32string_view());
    CHECK(RE32("\\w").match(U"\u010d") == U"\u010d");
    CHECK(RE32("\\p{Ll}").match(U"\u010d") == U"\u010d");
    CHECK(RE32("\\p{Ll}").match(U"\u010c") == u32string_view());
  }
  SUBCASE("groups") {
    vector<u32string_view> groups;
    CHECK(RE32(".(\\w).(\\w)").match(U"abcde", &groups) == U"abcd");
    CHECK(groups.size() == 2); CHECK(groups[0] == U"b"); CHECK(groups[1] == U"d");
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
    CHECK(groups[0] == U"\u010c");
    CHECK(groups[1] == U"\u010d");
  }
}

TEST_CASE("RE32::split") {
  vector<u32string_view> parts;
  CHECK(RE32(",").split(U"a,b,c", parts) == 3);
  CHECK(parts.size() == 3); CHECK(parts[0] == U"a"); CHECK(parts[1] == U"b"); CHECK(parts[2] == U"c");

  CHECK(RE32(",").split(U"a,b,c", parts, 1) == 2);
  CHECK(parts.size() == 2); CHECK(parts[0] == U"a"); CHECK(parts[1] == U"b,c");

  CHECK(RE32(",").split(U",a,b,c,", parts) == 5);
  CHECK(parts.size() == 5); CHECK(parts[0] == U""); CHECK(parts[1] == U"a");
  CHECK(parts[2] == U"b"); CHECK(parts[3] == U"c"); CHECK(parts[4] == U"");

  CHECK(RE32("\\d+").split(U"a42b1c1234", parts) == 4);
  CHECK(parts.size() == 4); CHECK(parts[0] == U"a"); CHECK(parts[1] == U"b"); CHECK(parts[2] == U"c"); CHECK(parts[3] == U"");

  CHECK(RE32("(?<=[ab])\\d(?=b)").split(U"a1a2a3b4b6", parts) == 3);
  CHECK(parts.size() == 3); CHECK(parts[0] == U"a1a2a"); CHECK(parts[1] == U"b"); CHECK(parts[2] == U"b6");
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
}

} // namespace linpipe
