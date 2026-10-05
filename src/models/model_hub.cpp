// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstdlib>
#include <filesystem>

#include "models/model_hub.h"

namespace linpipe {

namespace {

std::filesystem::path env_path(const char* name) {
  const char* value = std::getenv(name);
  return value && *value ? std::filesystem::path(value) : std::filesystem::path();
}

} // namespace

ModelHub::ModelHub(const std::string& dir, const std::string& repo_url)
  : dir(dir.empty() ? default_dir() : dir),
    repo_url(repo_url.empty() ? std::string(default_repo_url) : repo_url) {}

//   Linux   -> $XDG_CACHE_HOME/linpipe/model_hub, or ~/.cache/linpipe/model_hub
//   Windows -> %LOCALAPPDATA%\linpipe\model_hub
//   macOS   -> ~/Library/Caches/linpipe/model_hub
std::string ModelHub::default_dir() {
  std::filesystem::path base;

#if defined(_WIN32)
  base = env_path("LOCALAPPDATA");
  if (base.empty()) {
    auto profile = env_path("USERPROFILE");
    if (!profile.empty()) base = profile / "AppData" / "Local";
  }
#elif defined(__APPLE__)
  auto home = env_path("HOME");
  if (!home.empty()) base = home / "Library" / "Caches";
#else
  base = env_path("XDG_CACHE_HOME");
  // The XDG spec says relative paths must be ignored.
  if (base.empty() || base.is_relative()) {
    auto home = env_path("HOME");
    base = home.empty() ? std::filesystem::path() : home / ".cache";
  }
#endif

  if (base.empty())
    throw LinpipeError("Cannot determine the default ModelHub directory, please specify it explicitly");

  return (base / "linpipe" / "model_hub").string();
}

} // namespace linpipe
