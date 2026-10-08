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
  enum {
    IGNORECASE = 1,
    DOTALL = 2,
    MULTILINE = 4,
  };

  class Span {
   public:
    int start, end;
    const char* subject;
    std::string_view str() const { return start >= 0 ? std::string_view(subject + start, end - start) : std::string_view(); }
    operator bool() const { return start >= 0; }
    operator std::string_view() const { return str(); }
  };
  using Spans = std::vector<Span>;

  class Match : public Span {
   public:
    Spans groups;
  };

  RE(std::string_view pattern, int options = 0);
  RE(RE&& other) noexcept;
  RE(const RE& other) = delete;
  RE& operator=(const RE& other) = delete;
  ~RE();

  bool match(std::string_view str, Match* match = nullptr);
  bool search(std::string_view str, Match* match = nullptr);
  size_t split(std::string_view str, Spans& parts, size_t max_splits = 0);
  size_t sub(std::string_view str, std::string_view replacement, std::string& output, size_t max_subs = 0);

 private:
  void* re_;
};

// Regular expressions operating on UTF-32.
class RE32 {
 public:
  enum {
    IGNORECASE = 1,
    DOTALL = 2,
    MULTILINE = 4,
  };

  class Span {
   public:
    int start, end;
    const char32_t* subject;
    std::u32string_view str() const { return start >= 0 ? std::u32string_view(subject + start, end - start) : std::u32string_view(); }
    operator bool() const { return start >= 0; }
    operator std::u32string_view() const { return str(); }
  };
  using Spans = std::vector<Span>;

  class Match : public Span {
   public:
    Spans groups;
  };

  RE32(std::string_view pattern, int options = 0);
  RE32(std::u32string_view pattern, int options = 0);
  RE32(RE32&& other) noexcept;
  RE32(const RE32& other) = delete;
  RE32& operator=(const RE32& other) = delete;
  ~RE32();

  bool match(std::u32string_view str, Match* match = nullptr);
  bool search(std::u32string_view str, Match* match = nullptr);
  size_t split(std::u32string_view str, Spans& parts, size_t max_splits = 0);
  size_t sub(std::u32string_view str, std::u32string_view replacement, std::u32string& output, size_t max_subs = 0);

 private:
  void* re_;
};

} // namespace linpipe
