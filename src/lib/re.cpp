// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <array>

#include "lib/re.h"
#include "oniguruma/oniguruma.h"
#include "unilib/utf.h"

// Right now, only UTF32-LE encoding is supported, even if Oniguruma does provide UTF32-BE.
#ifdef __BYTE_ORDER__
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "Only little endian systems are supported!");
#endif

namespace linpipe {

namespace {

// Private Oniguruma initialization
class REInit {
 public:
  static void initialize() {
    if (!initialized_) {
      onig_initialize(encodings_.data(), encodings_.size());
      initialized_ = true;
    }
  }

 private:
  REInit() {
    initialize();
  }
  ~REInit() {
    if (initialized_) {
      onig_end();
      initialized_ = false;
    }
  }
  inline static bool initialized_ = false;
  inline static array<OnigEncoding, 2> encodings_ = {ONIG_ENCODING_UTF8, ONIG_ENCODING_UTF32_LE};
  static REInit singleton;
};
REInit REInit::singleton;

// Private template RE methods
template<class Char>
basic_string_view<Char> match(OnigRegexType* re, basic_string_view<Char> str, vector<basic_string_view<Char>>* groups) {
  if (groups) groups->clear();

  OnigRegion region;
  if (groups)
    onig_region_init(&region);
  int r = onig_match(re, (UChar*)str.data(), (UChar*)(str.data() + str.size()),
                     (UChar*)str.data(), groups ? &region : nullptr, ONIG_OPTION_NONE);
  if (r >= 0) {
    if (groups) {
      groups->reserve(region.num_regs - 1);
      for (int i = 1; i < region.num_regs; i++)
        groups->push_back(str.substr(region.beg[i] / sizeof(Char), (region.end[i] - region.beg[i]) / sizeof(Char)));

      onig_region_free(&region, 0);
    }
    return basic_string_view<Char>(str.data(), r / sizeof(Char));
  }

  if (groups)
    onig_region_free(&region, 0);
  if (r != ONIG_MISMATCH) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r);
    LOG(ERROR, "RE::match: An error occurred during matching: " << s);
  }
  return basic_string_view<Char>();
}

template<class Char>
basic_string_view<Char> search(OnigRegexType* re, basic_string_view<Char> str, vector<basic_string_view<Char>>* groups) {
  if (groups) groups->clear();

  OnigRegion region;
  onig_region_init(&region);
  int r = onig_search(re, (UChar*)str.data(), (UChar*)(str.data() + str.size()),
                      (UChar*)str.data(), (UChar*)(str.data() + str.size()), &region, ONIG_OPTION_NONE);
  if (r >= 0) {
    if (groups) {
      groups->reserve(region.num_regs - 1);
      for (int i = 1; i < region.num_regs; i++)
        groups->push_back(str.substr(region.beg[i] / sizeof(Char), (region.end[i] - region.beg[i]) / sizeof(Char)));
    }
    basic_string_view<Char> result = str.substr(region.beg[0] / sizeof(Char), (region.end[0] - region.beg[0]) / sizeof(Char));
    onig_region_free(&region, 0);
    return result;
  }

  onig_region_free(&region, 0);
  if (r != ONIG_MISMATCH) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r);
    LOG(ERROR, "RE::search: An error occurred during searching: " << s);
  }
  return basic_string_view<Char>();
}

template<class Char>
int split(OnigRegexType* re, basic_string_view<Char> str, vector<basic_string_view<Char>>& parts, int max_splits) {
  parts.clear();

  OnigRegion region;
  onig_region_init(&region);

  int index = 0, splits = 0;
  while (true) {
    int r = onig_search(re, (UChar*)str.data(), (UChar*)(str.data() + str.size()),
                        (UChar*)(str.data() + index), (UChar*)(str.data() + str.size()), &region, ONIG_OPTION_NONE);
    if (r < 0) {
      if (r != ONIG_MISMATCH) {
        char s[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str((UChar* )s, r);
        LOG(ERROR, "RE::split: An error occurred during splitting: " << s);
      }
      break;
    }

    parts.push_back(str.substr(index, region.beg[0] / sizeof(Char) - index));
    index = region.end[0] / sizeof(Char);
    splits++;
    if (max_splits > 0 && splits >= max_splits)
      break;
  }
  onig_region_free(&region, 0);

  parts.push_back(str.substr(index));
  return splits + 1;
}

template<class Char>
int sub(OnigRegexType* re, basic_string_view<Char> str, basic_string_view<Char> replacement, basic_string<Char>& output, int max_subs) {
  output.clear();

  OnigRegion region;
  onig_region_init(&region);

  int index = 0, subs = 0;
  while (true) {
    int r = onig_search(re, (UChar*)str.data(), (UChar*)(str.data() + str.size()),
                        (UChar*)(str.data() + index), (UChar*)(str.data() + str.size()), &region, ONIG_OPTION_NONE);
    if (r < 0) {
      if (r != ONIG_MISMATCH) {
        char s[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str((UChar* )s, r);
        LOG(ERROR, "RE::split: An error occurred during splitting: " << s);
      }
      break;
    }

    output.append(str.substr(index, region.beg[0] / sizeof(Char) - index));
    for (unsigned i = 0; i < replacement.size(); i++)
      if (replacement[i] == '\\' && i + 1 < replacement.size() && replacement[i + 1] >= '1' && replacement[i + 1] <= '9') {
        unsigned group = 0, j = i + 1;
        while (j < replacement.size() && replacement[j] >= '0' && replacement[j] <= '9')
          group = group * 10 + replacement[j++] - '0';
        if (group < (unsigned)region.num_regs) {
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
    subs++;
    if (max_subs > 0 && subs >= max_subs)
      break;
  }
  onig_region_free(&region, 0);

  output.append(str.substr(index));
  return subs;
}

}

// RE declarations
RE::RE(string_view pattern, int options) : re_(nullptr) {
  REInit::initialize();

  OnigErrorInfo einfo;
  int r = onig_new((OnigRegexType**)&re_, (UChar*)pattern.data(), (UChar*)(pattern.data() + pattern.size()),
                   ((options & IGNORECASE) ? ONIG_OPTION_IGNORECASE : 0)
                   | ((options & DOTALL) ? ONIG_OPTION_MULTILINE : 0)
                   | ((options & MULTILINE) ? ONIG_OPTION_NEGATE_SINGLELINE : 0),
                   ONIG_ENCODING_UTF8, ONIG_SYNTAX_PERL, &einfo);

  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar*)s, r, &einfo);
    throw LinpipeError{"RE::RE: Cannot parse regular expression '", pattern, "': ", s};
  }
}

RE::RE(RE&& other) : re_(other.re_) {
  other.re_ = nullptr;
}

RE::~RE() {
  if (re_) {
    onig_free((OnigRegexType*)re_);
    re_ = nullptr;
  }
}

string_view RE::match(string_view str, vector<string_view>* groups) {
  return linpipe::match<char>((OnigRegexType*)re_, str, groups);
}

string_view RE::search(string_view str, vector<string_view>* groups) {
  return linpipe::search<char>((OnigRegexType*)re_, str, groups);
}

int RE::split(string_view str, vector<string_view>& parts, int max_splits) {
  return linpipe::split<char>((OnigRegexType*)re_, str, parts, max_splits);
}

int RE::sub(string_view str, string_view replacement, string& output, int max_subs) {
  return linpipe::sub((OnigRegexType*)re_, str, replacement, output, max_subs);
}

// RE32 declarations
RE32::RE32(string_view pattern, int options) : RE32([pattern]{ u32string u32; unilib::utf::decode(pattern, u32); return u32; }(), options) {}

RE32::RE32(u32string_view pattern, int options) : re_(nullptr) {
  REInit::initialize();

  OnigErrorInfo einfo;
  int r = onig_new((OnigRegexType**)&re_, (UChar*)pattern.data(), (UChar*)(pattern.data() + pattern.size()),
                   ((options & IGNORECASE) ? ONIG_OPTION_IGNORECASE : 0)
                   | ((options & DOTALL) ? ONIG_OPTION_MULTILINE : 0)
                   | ((options & MULTILINE) ? ONIG_OPTION_NEGATE_SINGLELINE : 0),
                   ONIG_ENCODING_UTF32_LE, ONIG_SYNTAX_PERL, &einfo);

  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar*)s, r, &einfo);
    string u8pattern; unilib::utf::encode(pattern, u8pattern);
    throw LinpipeError{"RE::RE: Cannot parse regular expression '", u8pattern, "': ", s};
  }
}

RE32::RE32(RE32&& other) : re_(other.re_) {
  other.re_ = nullptr;
}

RE32::~RE32() {
  if (re_) {
    onig_free((OnigRegexType*)re_);
    re_ = nullptr;
  }
}

u32string_view RE32::match(u32string_view str, vector<u32string_view>* groups) {
  return linpipe::match<char32_t>((OnigRegexType*)re_, str, groups);
}

u32string_view RE32::search(u32string_view str, vector<u32string_view>* groups) {
  return linpipe::search<char32_t>((OnigRegexType*)re_, str, groups);
}

int RE32::split(u32string_view str, vector<u32string_view>& parts, int max_splits) {
  return linpipe::split<char32_t>((OnigRegexType*)re_, str, parts, max_splits);
}

int RE32::sub(u32string_view str, u32string_view replacement, u32string& output, int max_subs) {
  return linpipe::sub((OnigRegexType*)re_, str, replacement, output, max_subs);
}


} // namespace linpipe
