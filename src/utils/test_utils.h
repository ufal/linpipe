// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <atomic>
#include <random>

#include "common.h"
#include "utils/env_utf8.h"
#include "utils/path_utf8.h"

namespace linpipe {

class TempDir {
 public:
  explicit TempDir(std::string_view prefix = "linpipe_test") {
    static std::atomic<unsigned> rand_num = std::random_device{}();
    auto temp_dir = std::filesystem::temp_directory_path();
    while (path_.empty()) {
      auto candidate = temp_dir / path_from_utf8(fmt::format("{}_{}", prefix, rand_num++));
      std::error_code ec;
      if (std::filesystem::create_directory(candidate, ec))
        path_ = std::move(candidate);
      else if (ec)
        throw LinpipeError("Cannot create directory {}: {}", path_to_utf8(candidate), ec.message());
    }
  }
  ~TempDir() {
    std::error_code ec;
    std::filesystem::remove_all(path_, ec);
  }

  TempDir(const TempDir&) = delete;
  TempDir& operator=(const TempDir&) = delete;

  const std::filesystem::path& path() const { return path_; }
  std::string utf8() const { return path_to_utf8(path_); }

 private:
  std::filesystem::path path_;
};

class EnvGuard {
 public:
  EnvGuard(const char* name, const char* value) : name_(name), old_value_(getenv_utf8(name)) {
    setenv_utf8(name, value);
  }
  ~EnvGuard() {
    old_value_ ? setenv_utf8(name_.c_str(), old_value_->c_str()) : unsetenv_utf8(name_.c_str());
  }

  EnvGuard(const EnvGuard&) = delete;
  EnvGuard& operator=(const EnvGuard&) = delete;

 private:
  std::string name_;
  std::optional<std::string> old_value_;
};

} // namespace linpipe
