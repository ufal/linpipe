// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <fstream>

#include "lib/download.h"
#include "lib/sha256.h"
#include "lib/xz_stream.h"
#include "utils/asset_downloader.h"
#include "utils/env_utf8.h"
#include "utils/file_lock.h"
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

  throw LinpipeError("Cannot determine the cache directory for '{}' assets, please set the {} or LINPIPE_CACHE_DIR environment variable", family_dirname, family_env);
}

std::filesystem::path asset_file_path(std::string_view family, std::string_view asset_name,
                                      const std::filesystem::path& asset_dir, const std::filesystem::path& json_name, const std::string& name) {
  auto file_name = path_from_utf8(name);
  if (name.empty() || file_name != file_name.filename() || file_name == "." || file_name == ".." || file_name == json_name || name.ends_with(".lock") || name.ends_with(".tmp"))
    throw LinpipeError("Invalid file name '{}' of {} {}", name, family, asset_name);
  return asset_dir / file_name;
}

bool is_downloaded_file(const std::filesystem::path& path, uint64_t size = {}) {
  std::error_code ec;
  if (!std::filesystem::is_regular_file(path, ec))
    return false;
  if (!size)
    return true;
  auto actual_size = std::filesystem::file_size(path, ec);
  return !ec && actual_size == size;
}

void download_to_file(std::string_view file_url, const std::filesystem::path& path, uint64_t size = {}, std::string_view sha256 = {}, std::string_view compression = {}, bool replace = false) {
  if (!compression.empty() && compression != "xz")
    throw LinpipeError("Unsupported compression {} of {}", compression, file_url);

  // First check, without the lock.
  if (!replace && is_downloaded_file(path, size))
    return;

  std::error_code ec;
  std::filesystem::create_directories(path.parent_path(), ec);
  if (ec)
    throw LinpipeError("Cannot create directory {}: {}", path_to_utf8(path.parent_path()), ec.message());

  // Lock path.lock.
  auto lock_path = path;
  lock_path += ".lock";
  FileLock lock(lock_path);

  // Second check, under the lock.
  if (!replace) {
    if (is_downloaded_file(path, size))
      return;
    if (std::filesystem::exists(path, ec))
      LOG(WARN, "File {} is not valid, downloading it again", path_to_utf8(path));
  }

  // Download to path.tmp.
  auto tmp_path = path;
  tmp_path += ".tmp";

  try {
    {
      std::ofstream file(tmp_path, std::ios::binary | std::ios::trunc);
      if (!file)
        throw LinpipeError("Cannot open file {}", path_to_utf8(tmp_path));

      if (compression.empty()) {
        download_url(file_url, file, true);
      } else {
        XZOStream decompressor(file, XZMode::DECOMPRESS);
        download_url(file_url, decompressor, true);
        decompressor.close();
        if (!decompressor)
          throw LinpipeError("Cannot decompress {}", file_url);
      }

      file.close();
      if (!file)
        throw LinpipeError("Cannot write file {}", path_to_utf8(tmp_path));
    }

    // Verify size and checksum, if given.
    if (size) {
      auto actual_size = std::filesystem::file_size(tmp_path, ec);
      if (ec)
        throw LinpipeError("Cannot get size of {}: {}", path_to_utf8(tmp_path), ec.message());
      if (actual_size != size)
        throw LinpipeError("File {} has size {} instead of {}", file_url, actual_size, size);
    }

    if (!sha256.empty()) {
      std::string expected(sha256);
      for (char& c : expected)
        if (c >= 'A' && c <= 'F') c = static_cast<char>(c - 'A' + 'a');
      auto actual_sha256 = SHA256::hexdigest(tmp_path);
      if (actual_sha256 != expected)
        throw LinpipeError("File {} has SHA-256 {} instead of {}", file_url, actual_sha256, expected);
    }

    // Move the file from tmp to target path.
    std::filesystem::rename(tmp_path, path, ec);
    if (ec)
      throw LinpipeError("Cannot rename {} to {}: {}", path_to_utf8(tmp_path), path_to_utf8(path), ec.message());
  } catch (...) {
    // On failure, remove path.tmp before unlocking.
    std::error_code remove_ec;
    std::filesystem::remove(tmp_path, remove_ec);
    throw;
  }
  // The lock is released by the FileLock destructor.
}

} // namespace

void AssetDownloader::download_asset(std::string_view asset_name) {
  auto asset_dir = cache_dir() / path_from_utf8(asset_name);
  auto json_name = path_from_utf8(fmt::format("{}.json", family_));
  auto json_path = asset_dir / json_name;

  // Download the asset JSON.
  if (!is_downloaded_file(json_path)) {
    const auto& entries = json_get_object(assets_json(), "entries");
    auto entry = entries.find(asset_name);
    if (entry == entries.end())
      throw LinpipeError("Unknown {} {}", family_, asset_name);

    const auto& asset_json_url = json_get<const std::string&>(*entry, "url");
    download_to_file(asset_json_url, json_path);
  }
  auto json = json_parse_path(json_path);

  // Download all files of the asset.
  for (auto& file : json_get_array(json, "files")) {
    auto name = json_extract<std::string>(file, "name");
    auto file_path = asset_file_path(family_, asset_name, asset_dir, json_name, name);

    auto url = json_extract<std::string>(file, "url");
    auto size = json_extract<uint64_t>(file, "size");
    auto sha256 = json_extract<std::string>(file, "sha256");
    auto compression = file.contains("compression") ? json_extract<std::string>(file, "compression") : std::string();

    download_to_file(url, file_path, size, sha256, compression);
  }
}

AssetDownloader::AssetDownloader(std::string assets_json_url, std::string family) : assets_json_url_(std::move(assets_json_url)), family_(std::move(family)) {}

const Json& AssetDownloader::assets_json() {
  std::call_once(assets_json_once_, [this] {
    auto path = cache_dir() / path_from_utf8(fmt::format("{}s.json", family_));
    // The index is the only mutable file, so it is always refreshed when online.
    if (!offline_mode())
      download_to_file(assets_json_url_, path, {}, {}, {}, true);
    assets_json_ = json_parse_path(path);
  });
  return assets_json_;
}

const std::filesystem::path& AssetDownloader::cache_dir() {
  std::call_once(cache_dir_once_, [this] {
    cache_dir_ = determine_cache_dirname(family_);
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
  std::vector<std::string> seen_assets;
  return get_downloaded_asset(asset_name, seen_assets);
}

std::optional<std::filesystem::path> AssetDownloader::get_downloaded_asset(std::string_view asset_name, std::vector<std::string>& seen_assets) {
  if (std::ranges::find(seen_assets, asset_name) != seen_assets.end())
    return cache_dir() / path_from_utf8(asset_name);
  seen_assets.emplace_back(asset_name);

  auto asset_dir = get_single_downloaded_asset(asset_name);
  if (!asset_dir)
    return std::nullopt;

  auto json_path = *asset_dir / path_from_utf8(fmt::format("{}.json", family_));
  auto json = json_parse_path(json_path);
  if (json.contains("requirements"))
    for (const auto& requirement : json_extract<std::vector<std::string>>(json, "requirements"))
      if (!get_downloaded_asset(requirement, seen_assets))
        return std::nullopt;
  return asset_dir;
}

// Lock-free: every file is published atomically, so an asset is complete exactly when
// its JSON and all files listed in it exist. A missing file is a normal state (the asset
// is not downloaded, or is being downloaded); a size mismatch indicates damage.
std::optional<std::filesystem::path> AssetDownloader::get_single_downloaded_asset(std::string_view asset_name) {
  auto asset_dir = cache_dir() / path_from_utf8(asset_name);
  auto json_name = path_from_utf8(fmt::format("{}.json", family_));
  auto json_path = asset_dir / json_name;
  if (!is_downloaded_file(json_path))
    return std::nullopt;

  auto json = json_parse_path(json_path);
  for (auto& file : json_get_array(json, "files")) {
    auto file_name = json_extract<std::string>(file, "name");
    auto file_path = asset_file_path(family_, asset_name, asset_dir, json_name, file_name);

    std::error_code ec;
    if (!std::filesystem::is_regular_file(file_path, ec))
      return std::nullopt;

    auto expected_size = json_extract<uint64_t>(file, "size");
    auto actual_size = std::filesystem::file_size(file_path, ec);
    if (ec) {
      LOG(ERROR, "Cannot get size of file {} of asset {}", file_name, asset_name);
      return std::nullopt;
    }
    if (actual_size != expected_size) {
      LOG(ERROR, "File {} of asset {} has size {} instead of {}", file_name, asset_name, actual_size, expected_size);
      return std::nullopt;
    }
  }

  return asset_dir;
}

std::filesystem::path AssetDownloader::get_asset(std::string_view asset_name) {
  std::vector<std::string> seen_assets;
  return get_asset(asset_name, seen_assets);
}

std::filesystem::path AssetDownloader::get_asset(std::string_view asset_name, std::vector<std::string>& seen_assets) {
  if (std::ranges::find(seen_assets, asset_name) != seen_assets.end())
    return cache_dir() / path_from_utf8(asset_name);
  seen_assets.emplace_back(asset_name);

  auto asset_dir = get_single_asset(asset_name);

  auto json_path = asset_dir / path_from_utf8(fmt::format("{}.json", family_));
  auto json = json_parse_path(json_path);
  if (json.contains("requirements"))
    for (const auto& requirement : json_extract<std::vector<std::string>>(json, "requirements"))
      get_asset(requirement, seen_assets);
  return asset_dir;
}

std::filesystem::path AssetDownloader::get_single_asset(std::string_view asset_name) {
  if (auto path = get_single_downloaded_asset(asset_name); path)
    return path.value();

  if (offline_mode())
    throw LinpipeError("Cannot download missing asset in offline mode: {}", asset_name);

  download_asset(asset_name);

  if (auto path = get_single_downloaded_asset(asset_name); path)
    return path.value();

  throw LinpipeError("The downloaded {} {} in {} is not valid", family_, asset_name, path_to_utf8(cache_dir() / path_from_utf8(asset_name)));
}

} // namespace linpipe
