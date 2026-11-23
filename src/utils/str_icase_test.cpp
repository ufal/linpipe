// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest/doctest.h"
#include "utils/str_icase.h"

namespace linpipe {

TEST_CASE("str_ascii_icase") {
  CHECK(str_ascii_icase_eq("Hello", "hELLO"));
  CHECK(!str_ascii_icase_eq("a", "ab"));
  CHECK(!str_ascii_icase_eq("\u010C", "\u010D"));  // C with caron
  CHECK(str_ascii_icase_lt("apple", "Banana"));
  CHECK(str_ascii_icase_le("ABC", "abc"));
  CHECK(str_ascii_icase_ge("ABC", "abc"));
  CHECK(str_ascii_icase_gt("abc", "AB"));
  CHECK(str_ascii_icase_cmp("Abc", "aBD") == std::weak_ordering::less);
}

TEST_CASE("str_unicode_icase") {
  CHECK(str_unicode_icase_eq("\u010Ce\u0161tina", "\u010DE\u0160TINA"));  // Cestina
  CHECK(str_unicode_icase_eq("\u03A3\u039F\u03A3", "\u03C3\u03BF\u03C2"));  // Greek SOS with a final sigma
  CHECK(!str_unicode_icase_eq("stra\u00DFe", "STRASSE"));  // SS not equal to sharp s (just simple case folding is used)
  CHECK(str_unicode_icase_lt("\u010CAJ", "\u010Daje"));  // Caj
  CHECK(str_unicode_icase_le("\u017D\u00C1BA", "\u017E\u00E1ba"));  // Zaba
  CHECK(str_unicode_icase_ge("\u017D\u00C1BA", "\u017E\u00E1ba"));
  CHECK(str_unicode_icase_gt("\u017E\u00E1ba", "\u017DAba"));
  CHECK(str_unicode_icase_cmp("D\u016Fm", "\u010Daj") == std::weak_ordering::less);  // code point comparison, not collation

  CHECK(str_unicode_icase_starts_with("UDPipe 2", "udpipe"));
  CHECK(str_unicode_icase_starts_with("\u010Ce\u0161tina", "\u010DE\u0160"));  // Cestina
  CHECK(str_unicode_icase_starts_with("abc", "ABC"));
  CHECK(str_unicode_icase_starts_with("abc", ""));
  CHECK(str_unicode_icase_starts_with("", ""));
  CHECK(!str_unicode_icase_starts_with("ab", "abc"));
  CHECK(!str_unicode_icase_starts_with("", "a"));
  CHECK(!str_unicode_icase_starts_with("xabc", "abc"));
  CHECK(str_unicode_icase_starts_with("\u212Aelvin", "k"));  // Kelvin sign (3 bytes) folds to k (1 byte)
  CHECK(str_unicode_icase_starts_with("kelvin", "\u212A"));
  CHECK(str_unicode_icase_starts_with("\u03A3\u039F\u03A3 \u03C3\u03BF\u03C2", "\u03C3\u03BF\u03C2"));  // Greek SOS with a final sigma
  CHECK(!str_unicode_icase_starts_with("stra\u00DFe", "STRASS"));  // SS not equal to sharp s
}

} // namespace linpipe
