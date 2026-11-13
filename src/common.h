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
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <source_location>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <lib/fmt/ranges.h>
#include <lib/json/json_fwd.h>

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
 private:
  template<typename... Args>
  struct FormatWithLocation {
    FMT_CONSTEVAL FormatWithLocation(const char* format, std::source_location location = std::source_location::current())
        : format(format), location(location) {}
    fmt::format_string<Args...> format;
    std::source_location location;
  };

 public:
  template<typename... Args>
  explicit LinpipeError(FormatWithLocation<std::type_identity_t<Args>...> format, Args&&... args)
      : text_(fmt::format(format.format, std::forward<Args>(args)...)), location_(format.location) {}

  template<typename... Args>
  LinpipeError(std::source_location location, fmt::format_string<Args...> format, Args&&... args)
      : text_(fmt::format(format, std::forward<Args>(args)...)), location_(location) {}

  const char* what() const noexcept override { return text_.c_str(); }
  const std::source_location& location() const noexcept { return location_; }

 private:
  std::string text_;
  std::source_location location_;
};

// Logging
enum class LoggingLevel : int {
  LEVEL_TRACE = 0,
  LEVEL_INFO = 1,
  LEVEL_WARN = 2,
  LEVEL_ERROR = 3,
  LEVEL_FATAL = 4,
  LEVEL_PROGRESS = 5,  // not a level, controlled by logging_progress
};
extern LoggingLevel logging_level;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
extern bool logging_progress;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void logging_write(LoggingLevel level, std::string_view message, std::source_location location = std::source_location::current());

#define LOG(level, ...)                                                                       \
  do {                                                                                        \
    if (linpipe::LoggingLevel::LEVEL_##level == linpipe::LoggingLevel::LEVEL_PROGRESS ?       \
            linpipe::logging_progress :                                                       \
            linpipe::logging_level <= linpipe::LoggingLevel::LEVEL_##level)                   \
      linpipe::logging_write(linpipe::LoggingLevel::LEVEL_##level, fmt::format(__VA_ARGS__)); \
  } while (false)

// Additional formatters for {fmt}
template<>
struct fmt::formatter<std::byte> : fmt::formatter<unsigned> {
  constexpr auto format(std::byte b, fmt::format_context& ctx) const {
    return fmt::formatter<unsigned>::format(static_cast<unsigned>(b), ctx);
  }
};

std::string path_to_utf8(const std::filesystem::path& path);
template<>
struct fmt::formatter<std::filesystem::path> : fmt::formatter<std::string> {
  auto format(const std::filesystem::path& path, fmt::format_context& ctx) const {
    return fmt::formatter<std::string>::format(path_to_utf8(path), ctx);
  }
};

} // namespace linpipe
