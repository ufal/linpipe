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

TEST_CASE("Languages::language_by_name") {
  Languages languages;

  SUBCASE("finds a known language by its exact name") {
    Language& english = languages.language_by_name("English");
    CHECK(english.name_ == "English");
    CHECK(english.iso639_1 == "en");
  }

  SUBCASE("throws on an unknown name") {
    CHECK_THROWS_AS(languages.language_by_name("Klingon"), LinpipeError);
  }

  SUBCASE("is case sensitive") {
    CHECK_THROWS_AS(languages.language_by_name("english"), LinpipeError);
  }
}

TEST_CASE("Languages::language_by_iso639_1") {
  Languages languages;

  SUBCASE("finds a known language by its Set 1 code") {
    Language& french = languages.language_by_iso639_1("fr");
    CHECK(french.name_ == "French");
    CHECK(french.iso639_1 == "fr");
  }

  SUBCASE("throws on an unknown code") {
    CHECK_THROWS_AS(languages.language_by_iso639_1("xx"), LinpipeError);
  }

  SUBCASE("throws on a Set 2/3 code, since that is not Set 1") {
    CHECK_THROWS_AS(languages.language_by_iso639_1("fra"), LinpipeError);
  }
}

TEST_CASE("Languages::language_by_non_iso_639_1") {
  Languages languages;

  SUBCASE("finds a language by one of its Set 2/3 codes") {
    Language& french = languages.language_by_non_iso_639_1("fra");
    CHECK(french.name_ == "French");
    CHECK(french.iso639_1 == "fr");
  }

  SUBCASE("finds the same language through either of its two Set 2 codes") {
    // French has both a T code ("fra") and a B code ("fre").
    Language& via_t = languages.language_by_non_iso_639_1("fra");
    Language& via_b = languages.language_by_non_iso_639_1("fre");
    CHECK(&via_t == &via_b);
  }

  SUBCASE("throws on an unknown code") {
    CHECK_THROWS_AS(languages.language_by_non_iso_639_1("xyz"), LinpipeError);
  }

  SUBCASE("throws on a Set 1 code, since that is not Set 2/3") {
    CHECK_THROWS_AS(languages.language_by_non_iso_639_1("fr"), LinpipeError);
  }
}

TEST_CASE("Languages: all three lookups agree on the same entry") {
  Languages languages;

  Language& by_name = languages.language_by_name("German");
  Language& by_iso639_1 = languages.language_by_iso639_1("de");
  Language& by_non_iso = languages.language_by_non_iso_639_1("deu");

  CHECK(&by_name == &by_iso639_1);
  CHECK(&by_name == &by_non_iso);
}

} // namespace linpipe
