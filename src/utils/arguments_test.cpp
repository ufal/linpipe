// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest.h"
#include "utils/arguments.h"

namespace linpipe {

TEST_CASE("Arguments::parse_operations") {
  Arguments args;
  vector<string_view> parsed;
  vector<string_view> gold;

  SUBCASE("parses single operation name") {
    gold.push_back(" -load");
    CHECK_NOTHROW(args.parse_operations(parsed, " -load"));
    CHECK(parsed == gold);
  }
}

} // namespace linpipe
