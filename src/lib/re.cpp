// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "lib/oniguruma/oniguruma.h"
#include "lib/re.h"
#include "lib/unilib/utf.h"

// Only UTF32-LE is supported.
#ifdef __BYTE_ORDER__
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "Only little endian systems are supported!");
#endif

namespace linpipe {

namespace {

// Private Oniguruma initialization
class REInit {
 private:
  REInit() {
    onig_initialize(encodings_.data(), encodings_.size());
  }
  ~REInit() {
    onig_end();
  }
  inline static std::array<OnigEncoding, 2> encodings_ = {ONIG_ENCODING_UTF8, ONIG_ENCODING_UTF32_LE};
  static REInit singleton;
};
REInit REInit::singleton;

// Private template RE methods
template<typename Char, typename Match>
bool oniguruma_match(OnigRegexType* re, std::basic_string_view<Char> str, Match* match) {
  if (match) {
    match->str = {};
    match->groups.clear();
  }

  OnigRegion region;
  onig_region_init(&region);
  int r = onig_match(
      re, reinterpret_cast<const UChar*>(str.data()), reinterpret_cast<const UChar*>(str.data() + str.size()),
      reinterpret_cast<const UChar*>(str.data()), match ? &region : nullptr, ONIG_OPTION_NONE);

  if (r >= 0) {
    if (match) {
      match->str = str.substr(0, r / sizeof(Char));
      match->groups.reserve(region.num_regs - 1);
      for (int i = 1; i < region.num_regs; i++)
        if (region.beg[i] == ONIG_REGION_NOTPOS)
          match->groups.emplace_back();
        else
          match->groups.push_back(str.substr(region.beg[i] / sizeof(Char), (region.end[i] - region.beg[i]) / sizeof(Char)));
    }

    onig_region_free(&region, 0);
    return true;
  }

  onig_region_free(&region, 0);
  if (r != ONIG_MISMATCH) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(reinterpret_cast<UChar*>(s), r);
    LOG(ERROR, "RE::match: An error occurred during matching: " << s);
  }
  return false;
}

template<typename Char, typename Match>
bool oniguruma_search(OnigRegexType* re, std::basic_string_view<Char> str, Match* match) {
  if (match) {
    match->str = {};
    match->groups.clear();
  }

  OnigRegion region;
  onig_region_init(&region);
  int r = onig_search(
      re, reinterpret_cast<const UChar*>(str.data()), reinterpret_cast<const UChar*>(str.data() + str.size()),
      reinterpret_cast<const UChar*>(str.data()), reinterpret_cast<const UChar*>(str.data() + str.size()),
      match ? &region : nullptr, ONIG_OPTION_NONE);
  if (r >= 0) {
    if (match) {
      match->str = str.substr(region.beg[0] / sizeof(Char), (region.end[0] - region.beg[0]) / sizeof(Char));
      match->groups.reserve(region.num_regs - 1);
      for (int i = 1; i < region.num_regs; i++)
        if (region.beg[i] == ONIG_REGION_NOTPOS)
          match->groups.emplace_back();
        else
          match->groups.push_back(str.substr(region.beg[i] / sizeof(Char), (region.end[i] - region.beg[i]) / sizeof(Char)));
    }

    onig_region_free(&region, 0);
    return true;
  }

  onig_region_free(&region, 0);
  if (r != ONIG_MISMATCH) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(reinterpret_cast<UChar*>(s), r);
    LOG(ERROR, "RE::search: An error occurred during searching: " << s);
  }
  return false;
}

template<class Char>
size_t oniguruma_split(OnigRegexType* re, std::basic_string_view<Char> str,
                       std::vector<std::basic_string_view<Char>>& parts, size_t max_splits) {
  parts.clear();

  OnigRegion region;
  onig_region_init(&region);

  size_t splits = 0, index = 0;
  bool empty_match = false;
  while (index + empty_match <= str.size()) {
    int r = onig_search(
        re, reinterpret_cast<const UChar*>(str.data()), reinterpret_cast<const UChar*>(str.data() + str.size()),
        reinterpret_cast<const UChar*>(str.data() + index + empty_match), reinterpret_cast<const UChar*>(str.data() + str.size()),
        &region, ONIG_OPTION_NONE);
    if (r < 0) {
      if (r != ONIG_MISMATCH) {
        char s[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str(reinterpret_cast<UChar*>(s), r);
        LOG(ERROR, "RE::split: An error occurred during splitting: " << s);
      }
      break;
    }

    parts.emplace_back(str.data() + index, region.beg[0] / sizeof(Char) - index);
    index = region.end[0] / sizeof(Char);
    empty_match = region.end[0] == region.beg[0];
    splits++;
    if (max_splits > 0 && splits >= max_splits)
      break;
  }
  onig_region_free(&region, 0);

  if (index < str.size() || index) {
    parts.emplace_back(str.data() + index, str.size() - index);
    splits++;
  }
  return splits;
}

template<class Char>
size_t oniguruma_sub(OnigRegexType* re, std::basic_string_view<Char> str, std::basic_string_view<Char> replacement,
                     std::basic_string<Char>& output, size_t max_subs) {
  output.clear();

  OnigRegion region;
  onig_region_init(&region);

  size_t subs = 0, index = 0;
  bool empty_match = false;
  while (index + empty_match <= str.size()) {
    int r = onig_search(
        re, reinterpret_cast<const UChar*>(str.data()), reinterpret_cast<const UChar*>(str.data() + str.size()),
        reinterpret_cast<const UChar*>(str.data() + index + empty_match), reinterpret_cast<const UChar*>(str.data() + str.size()),
        &region, ONIG_OPTION_NONE);
    if (r < 0) {
      if (r != ONIG_MISMATCH) {
        char s[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str(reinterpret_cast<UChar*>(s), r);
        LOG(ERROR, "RE::split: An error occurred during splitting: " << s);
      }
      break;
    }

    output.append(str.substr(index, region.beg[0] / sizeof(Char) - index));
    for (size_t i = 0; i < replacement.size(); i++)
      if (replacement[i] == '\\' && i + 1 < replacement.size() && replacement[i + 1] >= '1' && replacement[i + 1] <= '9') {
        size_t group = 0, j = i + 1;
        while (j < replacement.size() && replacement[j] >= '0' && replacement[j] <= '9')
          group = group * 10 + replacement[j++] - '0';
        if (group < static_cast<size_t>(region.num_regs) && region.beg[group] != ONIG_REGION_NOTPOS) {
          output.append(str.substr(region.beg[group] / sizeof(Char), (region.end[group] - region.beg[group]) / sizeof(Char)));
          i = j - 1;
        } else {
          output.push_back('\\');
        }
      } else if (replacement[i] == '\\' && i + 1 < replacement.size() && replacement[i + 1] == '\\') {
        output.push_back('\\');
        i++;
      } else {
        output.push_back(replacement[i]);
      }
    index = region.end[0] / sizeof(Char);
    empty_match = region.end[0] == region.beg[0];
    subs++;
    if (max_subs > 0 && subs >= max_subs)
      break;
  }
  onig_region_free(&region, 0);

  output.append(str.substr(index));
  return subs;
}

} // namespace

// RE declarations
RE::RE(std::string_view pattern, REOptions options) {
  OnigErrorInfo einfo;
  int r = onig_new(
      reinterpret_cast<OnigRegexType**>(&re_),
      reinterpret_cast<const UChar*>(pattern.data()), reinterpret_cast<const UChar*>(pattern.data() + pattern.size()),
      (((options & IGNORECASE) != NONE) ? ONIG_OPTION_IGNORECASE : 0)
          | (((options & DOTALL) != NONE) ? ONIG_OPTION_MULTILINE : 0)
          | (((options & MULTILINE) != NONE) ? ONIG_OPTION_NEGATE_SINGLELINE : 0),
      ONIG_ENCODING_UTF8, ONIG_SYNTAX_PERL, &einfo);

  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(reinterpret_cast<UChar*>(s), r, &einfo);
    throw LinpipeError{"RE::RE: Cannot parse regular expression '", pattern, "': ", s};
  }
}

RE::RE(RE&& other) noexcept : re_(other.re_) {
  other.re_ = nullptr;
}

RE::~RE() {
  if (re_) {
    onig_free(reinterpret_cast<OnigRegexType*>(re_));
    re_ = nullptr;
  }
}

bool RE::match(std::string_view str, Match* match) {
  return oniguruma_match<char>(reinterpret_cast<OnigRegexType*>(re_), str, match);
}

bool RE::search(std::string_view str, Match* match) {
  return oniguruma_search<char>(reinterpret_cast<OnigRegexType*>(re_), str, match);
}

size_t RE::split(std::string_view str, std::vector<std::string_view>& parts, size_t max_splits) {
  return oniguruma_split<char>(reinterpret_cast<OnigRegexType*>(re_), str, parts, max_splits);
}

size_t RE::sub(std::string_view str, std::string_view replacement, std::string& output, size_t max_subs) {
  return oniguruma_sub(reinterpret_cast<OnigRegexType*>(re_), str, replacement, output, max_subs);
}

// RE32 declarations
RE32::RE32(std::string_view pattern, REOptions options) : RE32(unilib::utf::decoded(pattern), options) {}

RE32::RE32(std::u32string_view pattern, REOptions options) {
  OnigErrorInfo einfo;
  int r = onig_new(
      reinterpret_cast<OnigRegexType**>(&re_),
      reinterpret_cast<const UChar*>(pattern.data()), reinterpret_cast<const UChar*>(pattern.data() + pattern.size()),
      (((options & IGNORECASE) != NONE) ? ONIG_OPTION_IGNORECASE : 0)
          | (((options & DOTALL) != NONE) ? ONIG_OPTION_MULTILINE : 0)
          | (((options & MULTILINE) != NONE) ? ONIG_OPTION_NEGATE_SINGLELINE : 0),
      ONIG_ENCODING_UTF32_LE, ONIG_SYNTAX_PERL, &einfo);

  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(reinterpret_cast<UChar*>(s), r, &einfo);
    throw LinpipeError{"RE32::RE32: Cannot parse regular expression '", unilib::utf::encoded(pattern), "': ", s};
  }
}

RE32::RE32(RE32&& other) noexcept : re_(other.re_) {
  other.re_ = nullptr;
}

RE32::~RE32() {
  if (re_) {
    onig_free(reinterpret_cast<OnigRegexType*>(re_));
    re_ = nullptr;
  }
}

bool RE32::match(std::u32string_view str, Match* match) {
  return oniguruma_match<char32_t>(reinterpret_cast<OnigRegexType*>(re_), str, match);
}

bool RE32::search(std::u32string_view str, Match* match) {
  return oniguruma_search<char32_t>(reinterpret_cast<OnigRegexType*>(re_), str, match);
}

size_t RE32::split(std::u32string_view str, std::vector<std::u32string_view>& parts, size_t max_splits) {
  return oniguruma_split<char32_t>(reinterpret_cast<OnigRegexType*>(re_), str, parts, max_splits);
}

size_t RE32::sub(std::u32string_view str, std::u32string_view replacement, std::u32string& output, size_t max_subs) {
  return oniguruma_sub(reinterpret_cast<OnigRegexType*>(re_), str, replacement, output, max_subs);
}

} // namespace linpipe
