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

[[maybe_unused]]
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

void AssetDownloader::determine_cache_dir() {
  // 1. LINPIPE_<FAMILY>_DIR.
  std::string family_env = "LINPIPE_";
  for (char c : family_)
    family_env += c >= 'a' && c <= 'z' ? static_cast<char>(c - 'a' + 'A') : c;
  family_env += "_DIR";
  cache_dir_ = env_path(family_env);
  if (!cache_dir_.empty()) return;

  // 2. LINPIPE_CACHE_DIR/<family>.
  if (auto base = env_path("LINPIPE_CACHE_DIR"); !base.empty()) {
    cache_dir_ = base / path_from_utf8(family_);
    return;
  }

  // 3. <system cache dir>/<family>.
  if (auto base = system_cache_dir(); !base.empty()) {
    cache_dir_ = base / path_from_utf8(family_);
    return;
  }

  throw LinpipeError{"AssetDownloader: Cannot determine the cache directory for '", family_, "' assets, please set the ", family_env, " or LINPIPE_CACHE_DIR environment variable"};
}

AssetDownloader::AssetDownloader(std::string_view assets_json_url, std::string_view family) : assets_json_url_(assets_json_url), family_(family) {}

AssetDownloader::~AssetDownloader() = default;

std::filesystem::path AssetDownloader::get_asset(std::string_view asset_name) {
  {
    std::scoped_lock lock(assets_json_mutex_);
    if (!assets_json_) {
      determine_cache_dir();

      std::error_code ec;
      std::filesystem::create_directories(cache_dir_, ec);
      if (ec || !std::filesystem::is_directory(cache_dir_, ec))
        throw LinpipeError{"AssetDownloader: Cannot create cache directory '", path_to_utf8(cache_dir_), "'"};

      auto assets_json_path = cache_dir_ / assets_json_name;

      // Try updating the cached copy; when offline, fall back to it.
      std::string download_error;
      try {
        download_to_file(assets_json_url_, path_to_utf8(assets_json_path));
      } catch (const std::exception& e) {
        download_error = e.what();
      }

      std::ifstream is(assets_json_path);
      if (!is)
        throw LinpipeError{"AssetDownloader: Cannot download '", assets_json_url_, "' and no cached copy exists: ", download_error};
      if (!download_error.empty())
        LOG(WARN, "AssetDownloader: cannot update '" << assets_json_url_ << "', using the cached copy: " << download_error);

      try {
        assets_json_ = std::make_unique<Json>(Json::parse(is));
      } catch (const Json::exception& e) {
        throw LinpipeError{"AssetDownloader: The file '", path_to_utf8(assets_json_path), "' is not valid JSON: ", e.what()};
      }
    }

    // - Find asset_name in assets_json_, get asset_json_url and asset_path (under cache_dir_).
  }

  // - If asset_path already exists, return asset_path.
  // - download_asset(asset_json_url, asset_path);
  // - return asset_path

  (void)asset_name;
  return {};
}

} // namespace linpipe
