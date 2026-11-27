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

// Formatters for {fmt}, producing the syntax of parse_selectors
template<>
struct fmt::formatter<Selector> : fmt::formatter<std::string_view> {
  auto format(const Selector& selector, fmt::format_context& ctx) const {
    std::string text;
    auto append_escaped = [&text](std::string_view part) {
      for (char c : part) text.append(c == ':' ? 2 : 1, c);
    };
    if (!selector.key.empty()) {
      append_escaped(selector.key);
      text.push_back('=');
    }
    append_escaped(selector.value);
    return fmt::formatter<std::string_view>::format(text, ctx);
  }
};

// Ranges of selectors are not formatted by the generic range formatter, but by the one below.
template<std::ranges::input_range R>
  requires std::same_as<std::ranges::range_value_t<R>, Selector>
struct fmt::range_format_kind<R, char> : std::integral_constant<fmt::range_format, fmt::range_format::disabled> {};

template<std::ranges::input_range R>
  requires std::same_as<std::ranges::range_value_t<R>, Selector>
struct fmt::formatter<R> : fmt::formatter<std::string_view> {
  auto format(const R& selectors, fmt::format_context& ctx) const {
    return fmt::formatter<std::string_view>::format(fmt::to_string(fmt::join(selectors, ":")), ctx);
  }
};

} // namespace linpipe
