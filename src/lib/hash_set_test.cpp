// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest/doctest.h"
#include "lib/hash_set.h"

namespace linpipe {

TEST_CASE("HashSet uint64_t") {
  HashSet<uint64_t> set;

  for (uint64_t i = 1; i < 100; i += 2)
    set.insert(i);

  for (uint64_t i = 0; i < 100; i++)
    CHECK(set.contains(i) == (i % 2 == 1));
}

TEST_CASE("HashSet string") {
  HashSet<std::string> set;

  // Insert using different string types.
  set.insert("key_1");
  set.insert("key_3"s);
  set.insert("key_5"sv);
  for (uint64_t i = 7; i < 100; i += 2)
    set.insert(fmt::format("key_{}", i));

  // Search using different string types (different from insertion for 1, 3, 5).
  CHECK(set.contains("key_1"s));
  CHECK(set.contains("key_3"sv));
  CHECK(set.contains("key_5"));
  for (uint64_t i = 0; i < 100; i++) {
    CHECK(set.contains(fmt::format("key_{}", i)) == (i % 2 == 1));
    CHECK(set.contains(fmt::format("key_{}", i).c_str()) == (i % 2 == 1));
    CHECK(set.contains(std::string_view(fmt::format("key_{}", i))) == (i % 2 == 1));
  }
}

} // namespace linpipe
