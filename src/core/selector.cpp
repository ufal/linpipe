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
#include "utils/string_pair_parser.h"

namespace linpipe {

bool Selector::matches(std::string_view candidate) const {
  if (key == "language") return candidate == Languages::canonicalize_code_or_name(value);
  return candidate == value;
}

std::vector<Selector> parse_selectors(std::string_view input, std::source_location location) {
  return parse_string_pairs<Selector>(input, location);
}

} // namespace linpipe
