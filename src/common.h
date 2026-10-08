// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

// Headers available in all sources
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "lib/fmt/ranges.h"
#include "lib/json/json_fwd.h"

namespace linpipe {

using namespace std::literals;

// Configuration of JSON for Modern C++
using Json = nlohmann::json;

// Assert that int is at least 4B
static_assert(sizeof(int) >= sizeof(int32_t), "Int must be at least 4B wide!");

// Assert that we are on a little endian system
#ifdef __BYTE_ORDER__
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "Only little endian systems are supported!");
#endif

// Errors
class LinpipeError : public std::exception {
 public:
  LinpipeError(const std::string_view text) : text_(text) {}
  LinpipeError(std::initializer_list<std::string_view> texts) {
    for (auto&& text : texts) text_ += text;
  }
  const char* what() const noexcept override { return text_.c_str(); }

 private:
  std::string text_;
};

// Additional formatters for {fmt}
template<>
struct fmt::formatter<std::byte> : fmt::formatter<unsigned> {
  constexpr auto format(std::byte b, fmt::format_context& ctx) const {
    return fmt::formatter<unsigned>::format(unsigned(b), ctx);
  }
};

} // namespace linpipe
