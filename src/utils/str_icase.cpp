// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "lib/unilib/unicode.h"
#include "lib/unilib/utf.h"
#include "utils/str_icase.h"

namespace linpipe {

namespace {

unsigned char ascii_lower(unsigned char c) {
  return c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
}

char32_t unicode_fold(char32_t chr) {
  // Going through uppercase also unifies variants like final sigma or long s.
  return unilib::unicode::lowercase(unilib::unicode::uppercase(chr));
}

} // namespace

std::weak_ordering str_ascii_icase_cmp(std::string_view a, std::string_view b) {
  return std::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end(), [](unsigned char x, unsigned char y) {
    return std::weak_ordering(ascii_lower(x) <=> ascii_lower(y));
  });
}

bool str_ascii_icase_eq(std::string_view a, std::string_view b) {
  return a.size() == b.size() && std::ranges::equal(a, b, {}, ascii_lower, ascii_lower);
}

std::weak_ordering str_unicode_icase_cmp(std::string_view a, std::string_view b) {
  while (!a.empty() && !b.empty())
    if (auto x = unicode_fold(unilib::utf::decode(a)), y = unicode_fold(unilib::utf::decode(b)); x != y)
      return x <=> y;
  return a.size() <=> b.size();  // at least one of them is empty
}

bool str_unicode_icase_starts_with(std::string_view str, std::string_view prefix) {
  while (!prefix.empty())
    if (str.empty() || unicode_fold(unilib::utf::decode(str)) != unicode_fold(unilib::utf::decode(prefix)))
      return false;
  return true;
}

} // namespace linpipe
