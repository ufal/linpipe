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
  AssetDownloader(std::string assets_json_url, std::string family);

  // Lazy accessors
  const Json& assets_json();
  const std::filesystem::path& cache_dir();
  bool offline_mode();

  // Non-commital, offline, non-downloading asset getter
  std::optional<std::filesystem::path> get_downloaded_asset(std::string_view asset_name);

  std::filesystem::path get_asset(std::string_view asset_name);

 private:
  std::string assets_json_url_;
  std::string family_;  // "model", "lib"

  // Lazily initialized
  std::filesystem::path cache_dir_;
  Json assets_json_;
  bool offline_mode_ = false;
  std::once_flag assets_json_once_, cache_dir_once_, offline_mode_once_;
};

} // namespace linpipe
