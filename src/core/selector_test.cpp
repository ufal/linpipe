// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "core/selector.h"
#include "lib/doctest/doctest.h"

namespace linpipe {

TEST_CASE("Selector::matches") {
  CHECK(Selector{"name", "text"}.matches("text"));
  CHECK_FALSE(Selector{"name", "text"}.matches("other"));
  CHECK_FALSE(Selector{"name", "fr"}.matches("french"));

  CHECK(Selector{"language", "fr"}.matches("fr"));
  CHECK(Selector{"language", "FRA"}.matches("fr"));
  CHECK(Selector{"language", "French"}.matches("fr"));
  CHECK_FALSE(Selector{"language", "French"}.matches("de"));
}

TEST_CASE("parse_selectors") {
  auto selectors = parse_selectors("plain_text:language=fr");
  REQUIRE(selectors.size() == 2);
  CHECK(selectors[0].key == "");
  CHECK(selectors[0].value == "plain_text");
  CHECK(selectors[1].key == "language");
  CHECK(selectors[1].value == "fr");

  CHECK_THROWS_AS(parse_selectors("=fr"), LinPipeError);
}

} // namespace linpipe
