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
#include "utils/path_utf8.h"

namespace linpipe {

namespace {

constexpr std::string_view assets_json_name = "assets.json";

std::filesystem::path env_path(const std::string& name) {
  auto value = getenv_utf8(name.c_str());
  return value && !value->empty() ? path_from_utf8(*value) : std::filesystem::path();
}

std::filesystem::path system_cache_dir() {
  std::filesystem::path base;

#ifdef WIN32
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
  if (base.empty() || base.is_relative()) {
    auto home = env_path("HOME");
    base = home.empty() ? std::filesystem::path() : home / ".cache";
  }
#endif

  return base.empty() ? base : base / "linpipe";
}

std::filesystem::path determine_cache_dirname(std::string_view family) {
  // 1. LINPIPE_<FAMILY>_DIR.
  std::string family_env = "LINPIPE_";
  for (char c : family)
    family_env += c >= 'a' && c <= 'z' ? static_cast<char>(c - 'a' + 'A') : c;
  family_env += "_DIR";
  auto dir = env_path(family_env);
  if (!dir.empty())
    return dir;

  // 2. LINPIPE_CACHE_DIR/<family>.
  if (auto base = env_path("LINPIPE_CACHE_DIR"); !base.empty())
    return base / path_from_utf8(family);

  // 3. <system cache dir>/<family>.
  if (auto base = system_cache_dir(); !base.empty())
    return base / path_from_utf8(family);

  throw LinpipeError{"AssetDownloader: Cannot determine the cache directory for '", family, "' assets, please set the ", family_env, " or LINPIPE_CACHE_DIR environment variable"};
}

void download_to_file(std::string_view /*file_url*/, std::string_view /*path*/, uint64_t /*size*/ = {}, std::string_view /*sha256*/ = {}, std::string_view /*compression*/ = {}) {
}

[[maybe_unused]]
void download_asset(std::string_view /*asset_json_url*/, std::string_view /*path*/) {
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

AssetDownloader::AssetDownloader(std::string_view assets_json_url, std::string_view family) : assets_json_url_(assets_json_url), family_(family) {}

const Json& AssetDownloader::assets_json() {
  if (!assets_json_) {
    auto assets_json_path = cache_dir() / assets_json_name;

    if (!offline_mode())
      download_to_file(assets_json_url_, path_to_utf8(assets_json_path));

    std::ifstream is(assets_json_path);
    if (!is.is_open())
      throw LinpipeError{"AssetDownloader::assets_json: Cannot find ", path_to_utf8(assets_json_path)};

    {
      std::scoped_lock lock(mutex_);
      assets_json_ = Json::parse(is); // TODO: Replace with json_utils from PR
    }
  }

  return *assets_json_;
}

const std::filesystem::path& AssetDownloader::cache_dir() {
  if (!cache_dir_) {
    std::scoped_lock lock(mutex_);

    auto dirname = determine_cache_dirname(family_);

    std::error_code ec;
    std::filesystem::create_directories(dirname, ec);
    if (ec || !std::filesystem::is_directory(dirname, ec))
      throw LinpipeError{"AssetDownloader: Cannot create cache directory ", path_to_utf8(dirname)};

    cache_dir_ = dirname;
  }

  return *cache_dir_;
}

bool AssetDownloader::offline_mode() {
  // TODO

  return *offline_mode_;
}

std::optional<std::filesystem::path> AssetDownloader::get_downloaded_asset(std::string_view /*asset_name*/) {
  // TODO: Check if dir exists.
  // TODO: Read json, check files and file sizes (not sha256, too expensive).
  // TODO: Return path.

  return {};
}

std::filesystem::path AssetDownloader::get_asset(std::string_view asset_name) {
  if (auto path = get_downloaded_asset(asset_name); path) {
    return path.value();
  }

  if (offline_mode())
    throw LinpipeError{"Cannot download missing asset in offline mode: ", asset_name};

  // const auto& assets = assets_json();
  auto asset_dir = cache_dir() / asset_name;

  // - Find asset_name in assets, get corresponding asset_json_url.
  // - download_asset(asset_json_url, asset_dir);

  return asset_dir;
}

} // namespace linpipe
