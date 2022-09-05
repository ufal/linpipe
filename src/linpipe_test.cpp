// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "common.h"
#include "testing/testing.h"

namespace linpipe {

TEST_CASE("vectors can be sized and resized") {
  vector<int> v(5);

  REQUIRE(v.size() == 5);
  REQUIRE(v.capacity() >= 5);

  SUBCASE("adding to the vector increases it's size") {
    v.push_back(1);

    CHECK(v.size() == 6);
    CHECK(v.capacity() >= 6);
  }
  SUBCASE("reserving increases just the capacity") {
    v.reserve(6);

    CHECK(v.size() == 5);
    CHECK(v.capacity() >= 6);
  }
}

} // namespace linpipe
