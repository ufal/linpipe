// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace linpipe {

struct Selector {
  std::string key;
  std::string value;

  bool matches(std::string_view candidate, bool prefix_match = false) const;
};

std::vector<Selector> parse_selectors(std::string_view input, std::source_location location = std::source_location::current());

} // namespace linpipe
