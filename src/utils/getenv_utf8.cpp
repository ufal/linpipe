// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "getenv_utf8.h"

namespace linpipe {

std::optional<std::string> getenv_utf8(const char* name) {
  if (!name) return std::nullopt;

#ifndef _WIN32
  const auto* value = std::getenv(name);

  if (!value) return std::nullopt;
  return value;

#else
  int wname_len = MultiByteToWideChar(CP_UTF8, 0, name, -1, nullptr, 0);
  if (wname_len <= 0) return std::nullopt;

  std::wstring wname(wname_len - 1, L'\0');
  MultiByteToWideChar(CP_UTF8, 0, name, -1, wname.data(), wname_len);

  auto wvalue_len = GetEnvironmentVariableW(wname.c_str(), nullptr, 0);
  if (wvalue_len == 0) return std::nullopt;

  std::wstring wvalue;
  while (true) {
    wvalue.resize(wvalue_len - 1);

    SetLastError(ERROR_SUCCESS);
    auto new_len = GetEnvironmentVariableW(wname.c_str(), wvalue.data(), wvalue_len);
    if (new_len == 0) {
      if (GetLastError() != ERROR_SUCCESS) return std::nullopt;
      return "";
    }

    if (new_len < wvalue_len) {
      wvalue.resize(new_len);
      break;
    }
    wvalue_len = new_len;
  }

  int value_len = WideCharToMultiByte(CP_UTF8, 0, wvalue.c_str(), -1, nullptr, 0, nullptr, nullptr);
  if (value_len <= 0) return std::nullopt;

  std::optional<std::string> result(std::in_place, value_len - 1, '\0');
  WideCharToMultiByte(CP_UTF8, 0, wvalue.c_str(), -1, result->data(), value_len, nullptr, nullptr);
  return result;

#endif
}

} // namespace linpipe
