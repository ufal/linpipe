// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <compare>

#include "common.h"

namespace linpipe {

// ASCII case-insensitive comparison: A-Z are compared as a-z, all other bytes
// (including the non-ASCII ones) are compared as unsigned values.
std::weak_ordering str_ascii_icase_cmp(std::string_view a, std::string_view b);
bool str_ascii_icase_eq(std::string_view a, std::string_view b);
inline bool str_ascii_icase_lt(std::string_view a, std::string_view b) { return str_ascii_icase_cmp(a, b) < 0; }
inline bool str_ascii_icase_le(std::string_view a, std::string_view b) { return str_ascii_icase_cmp(a, b) <= 0; }
inline bool str_ascii_icase_gt(std::string_view a, std::string_view b) { return str_ascii_icase_cmp(a, b) > 0; }
inline bool str_ascii_icase_ge(std::string_view a, std::string_view b) { return str_ascii_icase_cmp(a, b) >= 0; }

// Unicode case-insensitive comparison of UTF-8 strings: the code points are
// compared after a simple (one-to-one) case folding, i.e., the uppercase and then
// the lowercase mapping, so for example the German sharp s and "ss" differ.
// The ordering is by the folded code points, not a locale collation.
// Invalid UTF-8 sequences are compared as the Unicode replacement character.
std::weak_ordering str_unicode_icase_cmp(std::string_view a, std::string_view b);
inline bool str_unicode_icase_eq(std::string_view a, std::string_view b) { return a == b || str_unicode_icase_cmp(a, b) == 0; }
inline bool str_unicode_icase_lt(std::string_view a, std::string_view b) { return str_unicode_icase_cmp(a, b) < 0; }
inline bool str_unicode_icase_le(std::string_view a, std::string_view b) { return str_unicode_icase_cmp(a, b) <= 0; }
inline bool str_unicode_icase_gt(std::string_view a, std::string_view b) { return str_unicode_icase_cmp(a, b) > 0; }
inline bool str_unicode_icase_ge(std::string_view a, std::string_view b) { return str_unicode_icase_cmp(a, b) >= 0; }

} // namespace linpipe
