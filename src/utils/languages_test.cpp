// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest/doctest.h"
#include "utils/languages.h"

namespace linpipe {

TEST_CASE("Languages::language_by_code") {
  SUBCASE("finds a known language by its Set 1 code") {
    const auto* french = Languages::language_by_code("fr");
    REQUIRE(french);
    CHECK(french->name == "French");
    CHECK(french->iso639_1 == "fr");
  }

  SUBCASE("finds a known language by its Set 3 code") {
    const auto* french = Languages::language_by_code("fra");
    REQUIRE(french);
    CHECK(french->name == "French");
    CHECK(french->iso639_1 == "fr");
  }

  SUBCASE("ignores the case of the code") {
    for (const auto* code : {"FR", "Fr", "FRA", "frA"}) {
      const auto* french = Languages::language_by_code(code);
      REQUIRE(french);
      CHECK(french->iso639_1 == "fr");
    }
  }

  SUBCASE("finds all known languages") {
    for (const auto& language : Languages::languages) {
      for (const auto& code : {language.iso639_1, language.iso639_2b, language.iso639_3}) {
        if (code) {
          const auto* found = Languages::language_by_code(code);
          REQUIRE(found);
          CHECK(found->iso639_3 == language.iso639_3);
        }
      }
    }
  }

  SUBCASE("returns nullptr on an unknown code") {
    REQUIRE(!Languages::language_by_code("x9"));
  }
}

TEST_CASE("Languages::language_by_name") {
  SUBCASE("finds a known language by its name") {
    const auto* french = Languages::language_by_name("French");
    REQUIRE(french);
    CHECK(french->iso639_1 == "fr");
  }

  SUBCASE("ignores the case of the name") {
    for (const auto* name : {"FRENCH", "french", "fReNcH"}) {
      const auto* french = Languages::language_by_name(name);
      REQUIRE(french);
      CHECK(french->iso639_1 == "fr");
    }
    const auto* abe = Languages::language_by_name("AB\u00C9");  // Abe with an acute e
    REQUIRE(abe);
    CHECK(abe->iso639_3 == "aba");
  }

  SUBCASE("finds all known languages") {
    for (const auto& language : Languages::languages) {
      const auto* found = Languages::language_by_name(language.name);
      REQUIRE(found);
      CHECK(found == &language);
    }
  }

  SUBCASE("returns nullptr on an unknown name") {
    REQUIRE(!Languages::language_by_name("Frenc"));
    REQUIRE(!Languages::language_by_name("Frenchh"));
  }
}

TEST_CASE("Languages::canonicalize_code_or_name") {
  CHECK(Languages::canonicalize_code_or_name("fre") == "fr");
  CHECK(Languages::canonicalize_code_or_name("FRA") == "fr");
  CHECK(Languages::canonicalize_code_or_name("french") == "fr");
  CHECK(Languages::canonicalize_code_or_name("FRENCH") == "fr");
  CHECK(Languages::canonicalize_code_or_name("Ancient Greek (to 1453)") == "grc");
  CHECK(Languages::canonicalize_code_or_name("x9") == "x9");
}

} // namespace linpipe
