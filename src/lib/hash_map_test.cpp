// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest/doctest.h"
#include "lib/hash_map.h"

namespace linpipe {

TEST_CASE("HashMap uint64_t") {
  HashMap<uint64_t, uint64_t> map;

  for (uint64_t i = 0; i < 100; i++)
    map.emplace(i, 2 * i + 1);

  for (uint64_t i = 0; i < 100; i++)
    CHECK(map[i] == 2 * i + 1);
}

TEST_CASE("HashMap string") {
  HashMap<std::string, uint64_t> map;

  // Insert using different string types.
  map.emplace("key_0", 2 * 0 + 1);
  map.emplace("key_1"s, 2 * 1 + 1);
  map.emplace("key_2"sv, 2 * 2 + 1);
  for (uint64_t i = 3; i < 100; i++)
    map.emplace(fmt::format("key_{}", i), 2 * i + 1);

  // Search using different string types (different from insertion for 0, 1, 2).
  CHECK(map.contains("key_0"s));
  CHECK(map.contains("key_1"sv));
  CHECK(map.contains("key_2"));
  for (uint64_t i = 0; i < 100; i++) {
    CHECK(map[fmt::format("key_{}", i)] == 2 * i + 1);
    CHECK(map[fmt::format("key_{}", i).c_str()] == 2 * i + 1);
    CHECK(map[std::string_view(fmt::format("key_{}", i))] == 2 * i + 1);
  }
}

} // namespace linpipe
