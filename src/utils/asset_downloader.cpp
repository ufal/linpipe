// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>

#include "lib/json/json.h"
#include "utils/asset_downloader.h"
#include "utils/getenv_utf8.h"
#include "utils/json_utils.h"
#include "utils/path_utf8.h"

namespace linpipe {

namespace {

std::optional<std::filesystem::path> env_path(const std::string& name) {
  auto value = getenv_utf8(name.c_str());
  if (!value || value->empty()) return std::nullopt;
  return path_from_utf8(*value);
}

std::optional<std::filesystem::path> system_cache_dir() {
#ifdef WIN32
  auto base = env_path("LOCALAPPDATA");
  if (!base) {
    if (auto profile = env_path("USERPROFILE")) base = *profile / "AppData" / "Local";
  }
#elif defined(__APPLE__)
  std::optional<std::filesystem::path> base;
  if (auto home = env_path("HOME")) base = *home / "Library" / "Caches";
#else
  auto base = env_path("XDG_CACHE_HOME");
  if (!base || base->is_relative()) {
    base.reset();
    if (auto home = env_path("HOME")) base = *home / ".cache";
  }
#endif

  if (base) *base /= "linpipe";
  return base;
}

std::filesystem::path determine_cache_dirname(std::string_view family) {
  std::string family_dirname = fmt::format("{}s", family);  // "model" -> "models", "lib" -> "libs"

  // 1. LINPIPE_<FAMILY>_DIR.
  std::string family_env = "LINPIPE_";
  for (char c : family_dirname)
    family_env += c >= 'a' && c <= 'z' ? static_cast<char>(c - 'a' + 'A') : c;
  family_env += "_DIR";
  if (auto dir = env_path(family_env))
    return *dir;

  // 2. LINPIPE_CACHE_DIR/<family_dirname>.
  if (auto base = env_path("LINPIPE_CACHE_DIR"))
    return *base / path_from_utf8(family_dirname);

  // 3. <system cache dir>/<family_dirname>.
  if (auto base = system_cache_dir())
    return *base / path_from_utf8(family_dirname);

  throw LinpipeError{"AssetDownloader: Cannot determine the cache directory for '", family_dirname, "' assets, please set the ", family_env, " or LINPIPE_CACHE_DIR environment variable"};
}

void download_to_file(std::string_view /*file_url*/, const std::filesystem::path& /*path*/, uint64_t /*size*/ = {}, std::string_view /*sha256*/ = {}, std::string_view /*compression*/ = {}) {
}

[[maybe_unused]]
void download_asset(std::string_view /*asset_json_url*/, const std::filesystem::path& /*path*/) {
    // Downloads the entire asset:
    // 1. Opens/creates path.lock and locks it exclusively (never delete it).
    // 2. If path/ exists, another thread/process already finished; unlock and return.
    // 3. Removes any stale path.tmp/ left over from a crashed run, creates it anew.
    // 4. Downloads the asset JSON from asset_json_url to path.tmp/.
    // 5. Downloads all files listed in the asset JSON to path.tmp/ using download_to_file().
    // 6. Renames path.tmp/ to path/ (path/ does not exist).
    // 7. Unlocks path.lock. On any failure, removes path.tmp/ before unlocking.
}

} // namespace

AssetDownloader::AssetDownloader(std::string assets_json_url, std::string family) : assets_json_url_(std::move(assets_json_url)), family_(std::move(family)) {}

const Json& AssetDownloader::assets_json() {
  std::call_once(assets_json_once_, [this] {
    auto path = cache_dir() / path_from_utf8(fmt::format("{}.json", family_));
    if (!offline_mode())
      download_to_file(assets_json_url_, path);
    assets_json_ = json_parse_path("AssetDownloader::assets_json", path);
  });
  return assets_json_;
}

const std::filesystem::path& AssetDownloader::cache_dir() {
  std::call_once(cache_dir_once_, [this] {
    auto dirname = determine_cache_dirname(family_);
    std::error_code ec;
    std::filesystem::create_directories(dirname, ec);
    if (ec || !std::filesystem::is_directory(dirname, ec))
      throw LinpipeError{"AssetDownloader: Cannot create cache directory ", path_to_utf8(dirname)};
    cache_dir_ = std::move(dirname);
  });
  return cache_dir_;
}

bool AssetDownloader::offline_mode() {
  std::call_once(offline_mode_once_, [this] {
    auto value = getenv_utf8("LINPIPE_OFFLINE");
    offline_mode_ = value && !value->empty() && *value != "0";
  });
  return offline_mode_;
}

std::optional<std::filesystem::path> AssetDownloader::get_downloaded_asset(std::string_view asset_name) {
  auto asset_dir = cache_dir() / path_from_utf8(asset_name);
  std::error_code ec;
  if (!std::filesystem::is_directory(asset_dir, ec)) return std::nullopt;
  // TODO: Read json, check files and file sizes (not sha256, too expensive).
  return asset_dir;
}

std::filesystem::path AssetDownloader::get_asset(std::string_view asset_name) {
  if (auto path = get_downloaded_asset(asset_name); path) {
    return path.value();
  }

  if (offline_mode())
    throw LinpipeError{"Cannot download missing asset in offline mode: ", asset_name};

  // const auto& assets = assets_json();
  auto asset_dir = cache_dir() / path_from_utf8(asset_name);

  // - Find asset_name in assets, get corresponding asset_json_url.
  // - download_asset(asset_json_url, asset_dir);

  return asset_dir;
}

} // namespace linpipe
