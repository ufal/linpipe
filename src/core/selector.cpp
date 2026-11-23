// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "core/selector.h"
#include "utils/languages.h"
#include "utils/str_icase.h"
#include "utils/string_pair_parser.h"

namespace linpipe {

bool Selector::matches(std::string_view candidate, bool prefix_match) const {
  std::string_view expected = value;
  std::string canonical_language;
  if (str_ascii_icase_eq(key, "language")) {
    canonical_language = Languages::canonicalize_code_or_name(value);
    expected = canonical_language;
  }

  return prefix_match ? str_unicode_icase_starts_with(candidate, expected) : str_unicode_icase_eq(candidate, expected);
}

std::vector<Selector> parse_selectors(std::string_view input, std::source_location location) {
  return parse_string_pairs<Selector>(input, location);
}

} // namespace linpipe
