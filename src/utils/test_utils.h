// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <filesystem>
#include <random>

#include "common.h"
#include "utils/path_utf8.h"

namespace linpipe {

class TempDir {
 public:
  explicit TempDir(std::string_view prefix = "linpipe_test", bool create = false) {
    constexpr int max_attempts = 100;
    std::random_device random;
    auto temp_dir = std::filesystem::temp_directory_path();
    for (int attempt = 0; attempt < max_attempts && root_.empty(); attempt++) {
      auto candidate = temp_dir / fmt::format("{}_{:08x}{:08x}", prefix, random(), random());
      std::error_code ec;
      if (std::filesystem::create_directory(candidate, ec))
        root_ = std::move(candidate);
      else if (ec)
        throw LinpipeError{"TempDir: Cannot create directory ", path_to_utf8(candidate), ": ", ec.message()};
    }
    if (root_.empty())
      throw LinpipeError{"TempDir: Cannot create a unique directory in ", path_to_utf8(temp_dir)};
    path_ = create ? root_ : root_ / "dir";
  }
  ~TempDir() {
    std::error_code ec;
    std::filesystem::remove_all(root_, ec);
  }

  TempDir(const TempDir&) = delete;
  TempDir& operator=(const TempDir&) = delete;

  const std::filesystem::path& path() const { return path_; }
  std::string utf8() const { return path_to_utf8(path_); }

 private:
  std::filesystem::path root_, path_;
};

class EnvGuard {
 public:
  EnvGuard(const char* name, const char* value) : name_(name) {
    if (const char* old = std::getenv(name)) old_value_ = old;
    set(name, value);
  }
  ~EnvGuard() { set(name_.c_str(), old_value_ ? old_value_->c_str() : nullptr); }

  EnvGuard(const EnvGuard&) = delete;
  EnvGuard& operator=(const EnvGuard&) = delete;

 private:
  static void set(const char* name, const char* value) {
#ifdef _WIN32
    _putenv_s(name, value ? value : "");  // An empty value removes the variable.
#else
    if (value)
      setenv(name, value, 1);
    else
      unsetenv(name);
#endif
  }

  std::string name_;
  std::optional<std::string> old_value_;
};

} // namespace linpipe
