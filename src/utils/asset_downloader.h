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
#include <mutex>

#include "common.h"

namespace linpipe {

class AssetDownloader {
 public:
  AssetDownloader(std::string_view assets_json_url, std::string_view family);

  // Lazy accessors
  const Json& assets_json();
  const std::filesystem::path& cache_dir();
  bool offline_mode();

  // Non-commital, offline, non-downloading, non-throwing asset getter
  std::optional<std::filesystem::path> get_downloaded_asset(std::string_view asset_name);

  std::filesystem::path get_asset(std::string_view asset_name);

 private:
  std::string assets_json_url_;
  std::string family_;  // "model", "lib"
  std::optional<std::filesystem::path> cache_dir_;
  std::optional<Json> assets_json_;
  std::mutex mutex_;
  std::optional<bool> offline_mode_;
};

} // namespace linpipe
