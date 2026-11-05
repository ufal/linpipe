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

#ifdef _WIN32
namespace {

std::optional<std::wstring> utf8_to_wide(const char* str) {
  int wstr_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, nullptr, 0);
  if (wstr_len <= 0)
    return std::nullopt;

  std::wstring wstr(wstr_len - 1, L'\0');
  MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, wstr.data(), wstr_len);
  return wstr;
}

} // namespace
#endif

std::optional<std::string> getenv_utf8(const char* name) {
  if (!name) return std::nullopt;

#ifndef _WIN32
  const auto* value = std::getenv(name);

  if (!value) return std::nullopt;
  return value;

#else
  auto wname = utf8_to_wide(name);
  if (!wname)
    return std::nullopt;

  auto wvalue_len = GetEnvironmentVariableW(wname->c_str(), nullptr, 0);
  if (wvalue_len == 0) return std::nullopt;

  std::wstring wvalue;
  while (true) {
    wvalue.resize(wvalue_len - 1);

    SetLastError(ERROR_SUCCESS);
    auto new_len = GetEnvironmentVariableW(wname->c_str(), wvalue.data(), wvalue_len);
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

bool setenv_utf8(const char* name, const char* value) {
  if (!name || !*name) return false;

#ifndef _WIN32
  return (value ? setenv(name, value, 1) : unsetenv(name)) == 0;

#else
  auto wname = utf8_to_wide(name);
  if (!wname) return false;

  if (!value) return SetEnvironmentVariableW(wname->c_str(), nullptr) || GetLastError() == ERROR_ENVVAR_NOT_FOUND;

  auto wvalue = utf8_to_wide(value);
  if (!wvalue) return false;
  return SetEnvironmentVariableW(wname->c_str(), wvalue->c_str());

#endif
}

bool unsetenv_utf8(const char* name) {
  if (!name || !*name) return false;

#ifndef _WIN32
  return unsetenv(name) == 0;

#else
  auto wname = utf8_to_wide(name);
  if (!wname) return false;

  return SetEnvironmentVariableW(wname->c_str(), nullptr) || GetLastError() == ERROR_ENVVAR_NOT_FOUND;
#endif
}

} // namespace linpipe
