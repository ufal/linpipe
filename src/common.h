// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

// Headers available in all sources
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "lib/json_fwd.h"

namespace linpipe {

using namespace std;

// Configuration of JSON for Modern C++
using Json = nlohmann::json;

// Configuration of cpp-httplib
#define CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_USE_CERTS_FROM_MACOSX_KEYCHAIN

// Assert that int is at least 4B
static_assert(sizeof(int) >= sizeof(int32_t), "Int must be at least 4B wide!");

// Assert that we are on a little endian system
#ifdef __BYTE_ORDER__
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "Only little endian systems are supported!");
#endif

// Logging
enum { LOGGING_TRACE=0, LOGGING_INFO=1, LOGGING_PROGRESS=2, LOGGING_WARN=3, LOGGING_ERROR=4, LOGGING_FATAL=5, };
extern int logging_level;
extern bool logging_to_file;
ostream& logging_start(int level, const char* file, int line);

#define LOG(level, message) do { \
    if constexpr (linpipe::LOGGING_##level == linpipe::LOGGING_PROGRESS) { \
      if (linpipe::logging_level <= linpipe::LOGGING_PROGRESS && !linpipe::logging_to_file) \
        linpipe::logging_start(linpipe::LOGGING_PROGRESS, __FILE__, __LINE__) << message << '\r'; \
    } else { \
      if (linpipe::logging_level <= linpipe::LOGGING_##level) \
        linpipe::logging_start(linpipe::LOGGING_##level, __FILE__, __LINE__) << message << std::endl; \
    } \
  } while(false)

// Errors
class LinpipeError : public exception {
 public:
  LinpipeError(const string_view text) : text_(text) {}
  LinpipeError(initializer_list<string_view> texts) {for (auto&& text : texts) text_ += text; }
  virtual const char* what() const noexcept override { return text_.c_str(); }
 private:
  string text_;
};

} // namespace linpipe
