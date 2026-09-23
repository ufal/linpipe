// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest.h"
#include "utils/languages.h"

namespace linpipe {

TEST_CASE("Languages::language_by_code") {
  Languages languages;

  SUBCASE("finds a known language by its Set 1 code") {
    auto french = languages.language_by_code("fr");
    REQUIRE(french);
    CHECK(french->name == "French");
    CHECK(french->iso639_1 == "fr");
  }

  SUBCASE("finds a known language by its Set 3 code") {
    auto french = languages.language_by_code("fra");
    REQUIRE(french);
    CHECK(french->name == "French");
    CHECK(french->iso639_1 == "fr");
  }

  SUBCASE("finds all known languages") {
    for (const auto& language : Languages::languages) {
      for (const auto& code : {language.iso639_1, language.iso639_2b, language.iso639_3}) {
        if (code) {
          auto found = languages.language_by_code(code);
          REQUIRE(found);
          CHECK(found->iso639_3 == language.iso639_3);
        }
      }
    }
  }

  SUBCASE("returns nullptr on an unknown code") {
    REQUIRE(!languages.language_by_code("x9"));
  }
}

} // namespace linpipe
