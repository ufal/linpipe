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
    CHECK_THROWS_AS(languages.language_by_name("Morporkian"), LinpipeError);
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
    CHECK_THROWS_AS(languages.language_by_iso639_1("x9"), LinpipeError);
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
    Language& via_t = languages.language_by_non_iso_639_1("fra");
    Language& via_b = languages.language_by_non_iso_639_1("fre");
    CHECK(&via_t == &via_b);
  }

  SUBCASE("throws on an unknown code") {
    CHECK_THROWS_AS(languages.language_by_non_iso_639_1("x9z"), LinpipeError);
  }

  SUBCASE("throws on a Set 1 code, since that is not Set 2/3") {
    CHECK_THROWS_AS(languages.language_by_non_iso_639_1("fr"), LinpipeError);
  }
}

TEST_CASE("Languages: languages without a Set 1 code (from SIL)") {
  Languages languages;

  const Language* non_iso1 = nullptr;
  for (const Language& language : allLanguages)
    if (language.iso639_1.empty()) {
      non_iso1 = &language;
      break;
    }

  REQUIRE(non_iso1 != nullptr);
  REQUIRE(!non_iso1->non_iso639_1_codes.empty());

  string name = non_iso1->name_;
  string code = non_iso1->non_iso639_1_codes[0];

  SUBCASE("is findable by name") {
    Language& found = languages.language_by_name(name);
    CHECK(found.iso639_1.empty());
    CHECK(found.name_ == name);
  }

  SUBCASE("is findable by one of its Set 2/3 codes") {
    Language& found = languages.language_by_non_iso_639_1(code);
    CHECK(found.name_ == name);
  }

  SUBCASE("both lookups resolve to the same object within one Languages instance") {
    Language& by_name = languages.language_by_name(name);
    Language& by_code = languages.language_by_non_iso_639_1(code);
    CHECK(&by_name == &by_code);
  }

  SUBCASE("is not findable via language_by_iso639_1, since it has no Set 1 code") {
    CHECK_THROWS_AS(languages.language_by_iso639_1(""), LinpipeError);
  }
}

TEST_CASE("Languages: languages only known via Universal Dependencies") {
  auto is_ud_provisional_code = [](const string& code) {
    return code.size() == 3 && code[0] == 'q' && code[1] >= 'a' && code[1] <= 't' && code[2] >= 'a' && code[2] <= 'z';
  };

  const Language* ud_only = nullptr;
  for (const Language& language : allLanguages)
    for (const string& code : language.non_iso639_1_codes)
      if (is_ud_provisional_code(code)) {
        ud_only = &language;
        break;
      }

  REQUIRE(ud_only != nullptr);

  Languages languages;
  string name = ud_only->name_;

  SUBCASE("has no Set 1 code") {
    CHECK(ud_only->iso639_1.empty());
  }

  SUBCASE("is findable by name") {
    Language& found = languages.language_by_name(name);
    CHECK(found.name_ == name);
  }
}

TEST_CASE("Languages: merge invariants across Wikipedia + SIL + UD") {
  SUBCASE("every language name is unique") {
    unordered_map<string, int> name_count;
    for (const Language& language : allLanguages)
      name_count[language.name_]++;

    string duplicates;
    for (const auto& [name, count] : name_count)
      if (count > 1)
        duplicates += name + " (" + std::to_string(count) + "x); ";

    INFO("duplicate names: " << duplicates);
    CHECK(duplicates.empty());
  }

  SUBCASE("every code (Set 1 or Set 2/3) belongs to exactly one language") {
    unordered_map<string, int> code_count;
    for (const Language& language : allLanguages) {
      if (!language.iso639_1.empty())
        code_count[language.iso639_1]++;
      for (const string& code : language.non_iso639_1_codes)
        code_count[code]++;
    }

    string duplicates;
    for (const auto& [code, count] : code_count)
      if (count > 1)
        duplicates += code + " (" + std::to_string(count) + "x); ";

    INFO("duplicate codes: " << duplicates);
    CHECK(duplicates.empty());
  }

  SUBCASE("iso639_1 is always empty or a 2-letter lowercase code") {
    auto is_lower_alpha = [](const string& s) {
      for (char c : s)
        if (c < 'a' || c > 'z')
          return false;
      return true;
    };

    for (const Language& language : allLanguages) {
      CHECK((language.iso639_1.empty() || (language.iso639_1.size() == 2 && is_lower_alpha(language.iso639_1))));
      for (const string& code : language.non_iso639_1_codes)
        CHECK(((code.size() == 2 || code.size() == 3) && is_lower_alpha(code)));
    }
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
