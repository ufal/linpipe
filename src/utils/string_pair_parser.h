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

// Parses colon-separated `key=value` or `value` items (the latter with an empty key);
// a double colon `::` denotes a literal colon.
template<typename T>
  requires std::constructible_from<T, std::string, std::string>
std::vector<T> parse_string_pairs(std::string_view input, std::source_location location = std::source_location::current()) {
  std::vector<T> pairs;

  std::string key, value;
  bool has_key = false;
  for (size_t i = 0; i <= input.size(); i++) {
    if (i == input.size() || (input[i] == ':' && (i + 1 == input.size() || input[i + 1] != ':'))) {
      if (has_key || !value.empty()) pairs.emplace_back(std::move(key), std::move(value));
      key.clear();
      value.clear();
      has_key = false;
    } else if (input[i] == ':') {
      value.push_back(input[i++]);
    } else if (input[i] == '=' && !has_key) {
      if (value.empty())
        throw LinpipeError(location, "Empty key before '=' in the key-value list '{}'", input);
      key = std::move(value);
      value.clear();
      has_key = true;
    } else {
      value.push_back(input[i]);
    }
  }
  return pairs;
}

} // namespace linpipe
