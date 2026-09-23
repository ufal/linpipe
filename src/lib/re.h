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

// Regular expressions operating on UTF-8.
class RE {
 public:
  enum { IGNORECASE = 1, DOTALL = 2, MULTILINE = 4 };

  RE(std::string_view pattern, int options=0);
  RE(RE&& other);
  RE(const RE& other) = delete;
  RE& operator=(const RE& other) = delete;
  ~RE();

  std::string_view match(std::string_view str, std::vector<std::string_view>* groups=nullptr);
  std::string_view search(std::string_view str, std::vector<std::string_view>* groups=nullptr);
  size_t split(std::string_view str, std::vector<std::string_view>& parts, size_t max_splits=0);
  size_t sub(std::string_view str, std::string_view replacement, std::string& output, size_t max_subs=0);

 private:
  void* re_;
};

// Regular expressions operating on UTF-32.
class RE32 {
 public:
  enum { IGNORECASE = 1, DOTALL = 2, MULTILINE = 4 };

  RE32(std::string_view pattern, int options=0);
  RE32(std::u32string_view pattern, int options=0);
  RE32(RE32&& other);
  RE32(const RE32& other) = delete;
  RE32& operator=(const RE32& other) = delete;
  ~RE32();

  std::u32string_view match(std::u32string_view str, std::vector<std::u32string_view>* groups=nullptr);
  std::u32string_view search(std::u32string_view str, std::vector<std::u32string_view>* groups=nullptr);
  size_t split(std::u32string_view str, std::vector<std::u32string_view>& parts, size_t max_splits=0);
  size_t sub(std::u32string_view str, std::u32string_view replacement, std::u32string& output, size_t max_subs=0);

 private:
  void* re_;
};

} // namespace linpipe
