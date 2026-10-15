// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "utils/asset_downloader.h"

namespace linpipe {

std::filesystem::path AssetDownloader::get_asset(std::string_view /*asset_name*/) {
  // 1. If !assets_json_, make assets_json_ from assets_json_url_.
  // 2. Find asset_name in assets_json_, get asset_json_url and asset_path.
  // 3. download_asset(asset_json_url, asset_path);
  // 4. return asset_path
  return {};
}

void AssetDownloader::download_asset(std::string_view /*asset_json_url*/, std::string_view /*path*/) {
    // Downloads the entire asset:
    // 1. Opens/creates path.lock and locks it exclusively (never delete it).
    // 2. If path/ exists, another thread/process already finished; unlock and return.
    // 3. Removes any stale path.tmp/ left over from a crashed run, creates it anew.
    // 4. Downloads the asset JSON from asset_json_url to path.tmp/.
    // 5. Downloads all files listed in the asset JSON to path.tmp/ using download_to_file().
    // 6. Renames path.tmp/ to path/ (path/ does not exist).
    // 7. Unlocks path.lock. On any failure, removes path.tmp/ before unlocking.
}

void AssetDownloader::download_to_file(std::string_view /*file_url*/, std::string_view /*path*/, uint64_t /*size*/, std::string_view /*sha256*/, std::string_view /*decompress*/) {
}

} // namespace linpipe
