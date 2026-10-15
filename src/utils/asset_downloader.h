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
  AssetDownloader(std::string_view assets_json_url, std::filesystem::path cache_dir = {});
  ~AssetDownloader();

  std::filesystem::path get_asset(std::string_view asset_name);

 private:
  void download_asset(std::string_view asset_json_url, std::string_view path);
  static void download_to_file(std::string_view file_url, std::string_view path, uint64_t size, std::string_view sha256, std::string_view compression = {});

  std::string assets_json_url_;
  std::filesystem::path cache_dir_;
  std::unique_ptr<Json> assets_json_;
  std::mutex assets_json_mutex_;
};

} // namespace linpipe
