// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstdlib>
#include <optional>

#include "lib/doctest/doctest.h"
#include "models/model_hub.h"

namespace linpipe {

namespace {

// Sets (or unsets, when value is nullptr) an environment variable and
// restores its original value on destruction.
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
    if (value) setenv(name, value, 1); else unsetenv(name);
#endif
  }

  std::string name_;
  std::optional<std::string> old_value_;
};

} // namespace

TEST_CASE("ModelHub") {
  SUBCASE("throws when the default directory cannot be determined") {
#if defined(_WIN32)
    EnvGuard local_app_data("LOCALAPPDATA", nullptr), user_profile("USERPROFILE", nullptr);
#elif defined(__APPLE__)
    EnvGuard home("HOME", nullptr);
#else
    EnvGuard xdg_cache_home("XDG_CACHE_HOME", nullptr), home("HOME", nullptr);
#endif
    CHECK_THROWS_AS(ModelHub{}, LinpipeError);
    CHECK_THROWS_WITH_AS(ModelHub{}, doctest::Contains("ModelHub directory"), LinpipeError);
  }

#if !defined(_WIN32) && !defined(__APPLE__)
  SUBCASE("ignores relative XDG_CACHE_HOME and throws without HOME") {
    EnvGuard xdg_cache_home("XDG_CACHE_HOME", "relative/cache"), home("HOME", nullptr);
    CHECK_THROWS_AS(ModelHub{}, LinpipeError);
  }
#endif

  SUBCASE("does not throw when the directory is given explicitly") {
#if defined(_WIN32)
    EnvGuard local_app_data("LOCALAPPDATA", nullptr), user_profile("USERPROFILE", nullptr);
#else
    EnvGuard xdg_cache_home("XDG_CACHE_HOME", nullptr), home("HOME", nullptr);
#endif
    CHECK_NOTHROW(ModelHub{"explicit/model_hub"});
  }
}

} // namespace linpipe
