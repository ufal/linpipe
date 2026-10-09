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
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "lib/fmt/ranges.h"
#include "lib/json/json_fwd.h"

namespace linpipe {

using namespace std::literals;

// Import size_t and ptrdiff_t and provide user-defined literals for them
using std::size_t, std::ptrdiff_t;

consteval size_t operator""_uz(unsigned long long n) {
  if (n > (std::numeric_limits<size_t>::max)()) throw "Literal out of range for std::size_t";
  return static_cast<size_t>(n);
}

consteval ptrdiff_t operator""_z(unsigned long long n) {
  if (n > static_cast<unsigned long long>((std::numeric_limits<ptrdiff_t>::max)())) throw "Literal out of range for std::ptrdiff_t";
  return static_cast<ptrdiff_t>(n);
}

// Import basic integer types with a fixed width
using std::int8_t, std::int16_t, std::int32_t, std::int64_t;
using std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t;

// Assert that int is at least 4B
static_assert(sizeof(int) >= sizeof(int32_t), "Int must be at least 4B wide!");

// Assert that we are on a little endian system
#ifdef __BYTE_ORDER__
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "Only little endian systems are supported!");
#endif

// A shortcut for JSON for Modern C++
using Json = nlohmann::json;

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

// Logging
enum class LoggingLevel : int {
  LEVEL_TRACE = 0,
  LEVEL_INFO = 1,
  LEVEL_PROGRESS = 2,
  LEVEL_WARN = 3,
  LEVEL_ERROR = 4,
  LEVEL_FATAL = 5,
};
extern LoggingLevel logging_level;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
extern bool logging_to_file;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

class LoggingStream {
 public:
  explicit LoggingStream(LoggingLevel level, const char* source, int line);
  ~LoggingStream();

  template<class T> LoggingStream& operator<<(const T& v) {
    buf_ << v;
    return *this;
  }

 private:
  std::ostringstream buf_;
};

// NOLINTBEGIN(bugprone-macro-parentheses,cppcoreguidelines-macro-usage)
#define LOG(level, message)                                                                                   \
  do {                                                                                                        \
    if constexpr (linpipe::LoggingLevel::LEVEL_##level == linpipe::LoggingLevel::LEVEL_PROGRESS) {            \
      if (linpipe::logging_level <= linpipe::LoggingLevel::LEVEL_PROGRESS && !linpipe::logging_to_file)       \
        linpipe::LoggingStream(linpipe::LoggingLevel::LEVEL_PROGRESS, __FILE__, __LINE__) << message << '\r'; \
    } else {                                                                                                  \
      if (linpipe::logging_level <= linpipe::LoggingLevel::LEVEL_##level)                                     \
        linpipe::LoggingStream(linpipe::LoggingLevel::LEVEL_##level, __FILE__, __LINE__) << message << '\n';  \
    }                                                                                                         \
  } while (false)
// NOLINTEND(bugprone-macro-parentheses,cppcoreguidelines-macro-usage)

// Additional formatters for {fmt}
template<>
struct fmt::formatter<std::byte> : fmt::formatter<unsigned> {
  constexpr auto format(std::byte b, fmt::format_context& ctx) const {
    return fmt::formatter<unsigned>::format(static_cast<unsigned>(b), ctx);
  }
};

} // namespace linpipe
