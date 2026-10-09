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

// Regular expression options.
enum class REOptions : unsigned {
  NONE = 0,
  IGNORECASE = 1,
  DOTALL = 2,
  MULTILINE = 4,
};

constexpr REOptions operator|(REOptions a, REOptions b) {
  return static_cast<REOptions>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
}
constexpr REOptions operator&(REOptions a, REOptions b) {
  return static_cast<REOptions>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
}

// Regular expressions operating on UTF-8.
class RE {
 public:
  using enum REOptions;

  class Match {
   public:
    std::string_view str;
    std::vector<std::string_view> groups;

    explicit operator bool() const { return str.data() != nullptr; }
    operator std::string_view() const { return str; }
  };

  RE(std::string_view pattern, REOptions options = NONE);
  RE(RE&& other) noexcept;
  RE(const RE& other) = delete;
  RE& operator=(const RE& other) = delete;
  ~RE();

  bool match(std::string_view str, Match* match = nullptr);
  bool search(std::string_view str, Match* match = nullptr);
  size_t split(std::string_view str, std::vector<std::string_view>& parts, size_t max_splits = 0);
  size_t sub(std::string_view str, std::string_view replacement, std::string& output, size_t max_subs = 0);

 private:
  void* re_;
};

// Regular expressions operating on UTF-32.
class RE32 {
 public:
  using enum REOptions;

  class Match {
   public:
    std::u32string_view str;
    std::vector<std::u32string_view> groups;

    explicit operator bool() const { return str.data() != nullptr; }
    operator std::u32string_view() const { return str; }
  };

  RE32(std::string_view pattern, REOptions options = NONE);
  RE32(std::u32string_view pattern, REOptions options = NONE);
  RE32(RE32&& other) noexcept;
  RE32(const RE32& other) = delete;
  RE32& operator=(const RE32& other) = delete;
  ~RE32();

  bool match(std::u32string_view str, Match* match = nullptr);
  bool search(std::u32string_view str, Match* match = nullptr);
  size_t split(std::u32string_view str, std::vector<std::u32string_view>& parts, size_t max_splits = 0);
  size_t sub(std::u32string_view str, std::u32string_view replacement, std::u32string& output, size_t max_subs = 0);

 private:
  void* re_;
};

} // namespace linpipe
