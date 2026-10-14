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

namespace {

constexpr std::string_view asset_schema_path = "../assets/asset.schema.json";

[[maybe_unused]] void download_to_file([[maybe_unused]] std::string_view file_url,
                                       [[maybe_unused]] std::string_view path,
                                       [[maybe_unused]] uint64_t size,
                                       [[maybe_unused]] std::string_view sha256,
                                       [[maybe_unused]] bool unpack) {
}

} // namespace

void download_asset([[maybe_unused]] std::string_view asset_json_url, [[maybe_unused]] std::string_view path) {
    // Downloads the entire asset:
    // 1. Opens/creates path.lock and locks it exclusively (never delete it).
    // 2. If path/ exists, another thread/process already finished; unlock and return.
    // 3. Removes any stale path.tmp/ left over from a crashed run, creates it anew.
    // 4. Downloads the asset JSON from asset_json_url to path.tmp/ and validates it.
    // 5. Downloads all files listed in the asset JSON to path.tmp/ using download_to_file().
    // 6. Renames path.tmp/ to path/ (path/ does not exist).
    // 7. Unlocks path.lock. On any failure, removes path.tmp/ before unlocking.
}

} // namespace linpipe
