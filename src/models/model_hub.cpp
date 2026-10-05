// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <filesystem>
#include <fstream>
#include <system_error>

#include "lib/httplib/httplib.h"
#include "models/model_hub.h"
#include "utils/getenv_utf8.h"
#include "utils/path_utf8.h"

namespace linpipe {

namespace {

std::filesystem::path env_path(const char* name) {
  auto value = getenv_utf8(name);
  return value && !value->empty() ? path_from_utf8(*value) : std::filesystem::path();
}

void download(const std::string& url, const std::filesystem::path& target) {
  auto scheme_end = url.find("://");
  if (scheme_end == std::string::npos)
    throw LinpipeError{"Cannot download '", url, "': the URL has no scheme"};
  auto path_start = url.find('/', scheme_end + 3);
  auto host = url.substr(0, path_start);
  auto path = path_start == std::string::npos ? std::string("/") : url.substr(path_start);

  httplib::Client client(host);
  if (!client.is_valid())
    throw LinpipeError{"Cannot download '", url, "': unsupported URL"};
  client.set_follow_location(true);

  auto res = client.Get(path);
  if (!res)
    throw LinpipeError{"Cannot download '", url, "': ", httplib::to_string(res.error())};
  if (res->status != 200)
    throw LinpipeError{"Cannot download '", url, "': HTTP status ", std::to_string(res->status)};

  auto target_tmp = target;
  target_tmp += ".tmp";
  {
    std::ofstream os(target_tmp, std::ios::binary);
    os.write(res->body.data(), res->body.size());
    os.close();
    if (!os)
      throw LinpipeError{"Cannot write file '", path_to_utf8(target_tmp), "'"};
  }

  std::error_code ec;
  std::filesystem::rename(target_tmp, target, ec);
  if (ec) {
    std::filesystem::remove(target_tmp, ec);
    throw LinpipeError{"Cannot create file '", path_to_utf8(target), "': ", ec.message()};
  }
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

  return path_to_utf8(base / "linpipe" / "model_hub");
}

Model* ModelHub::get_model([[maybe_unused]] const std::string& name) {
  ensure_local_repo();

  // TODO: Find the model in the repository JSON, download it if needed, and load it.
  return nullptr;
}

void ModelHub::ensure_local_repo() {
  auto dir_path = path_from_utf8(dir);

  std::error_code ec;
  std::filesystem::create_directories(dir_path, ec);
  if (ec || !std::filesystem::is_directory(dir_path, ec))
    throw LinpipeError{"Cannot create ModelHub directory '", dir, "'", ec ? ": " : "", ec ? ec.message() : ""};

  auto repo_json = dir_path / repo_json_name;
  if (!std::filesystem::exists(repo_json, ec))
    download(repo_url, repo_json);
}

} // namespace linpipe
