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
#include <iterator>

#include "lib/httplib/httplib.h"
#include "lib/json/json.h"
#include "models/model_hub.h"
#include "utils/getenv_utf8.h"
#include "utils/path_utf8.h"

namespace linpipe {

namespace {

std::filesystem::path env_path(const char* name) {
  /* Gets an environment variable as a path.

  Receives:
    name: name of the environment variable

  Returns:
    the variable value as a path, or an empty path if it is unset or empty
  */

  auto value = getenv_utf8(name);
  return value && !value->empty() ? path_from_utf8(*value) : std::filesystem::path();
}

std::string fetch(const std::string& url) {
  /* Downloads the given URL, following redirects.

  Receives:
    url: http:// or https:// URL

  Returns:
    the response body

  Throws:
    LinpipeError if the URL is invalid, the connection fails, or the response
      status is not 200.
  */

  auto scheme_end = url.find("://");
  if (scheme_end == std::string::npos)
    throw LinpipeError{"fetch: Cannot download '", url, "': the URL has no scheme"};
  auto path_start = url.find('/', scheme_end + 3);
  auto host = url.substr(0, path_start);
  auto path = path_start == std::string::npos ? std::string("/") : url.substr(path_start);

  httplib::Client client(host);
  if (!client.is_valid())
    throw LinpipeError{"fetch: Cannot download '", url, "': unsupported URL"};
  client.set_follow_location(true);
  // Fail fast when offline, instead of waiting for httplib's default 300s.
  client.set_connection_timeout(10);
  client.set_read_timeout(60);

  auto res = client.Get(path);
  if (!res)
    throw LinpipeError{"fetch: Cannot download '", url, "': ", httplib::to_string(res.error())};
  if (res->status != 200)
    throw LinpipeError{"fetch: Cannot download '", url, "': HTTP status ", std::to_string(res->status)};

  return std::move(res->body);
}

void write_atomically(const std::filesystem::path& target, std::string_view content) {
  /* Writes the content into the target file atomically.

  Receives:
    target: path of the file to write
    content: data to write

  Throws:
    LinpipeError if the file cannot be written; no temporary file is left
      behind in that case.
  */

  auto target_tmp = target;
  target_tmp += ".tmp";
  {
    std::ofstream os(target_tmp, std::ios::binary);
    os.write(content.data(), content.size());
    os.close();
    if (!os) {
      std::error_code ec;
      std::filesystem::remove(target_tmp, ec);
      throw LinpipeError{"write_atomically: Cannot write file '", path_to_utf8(target_tmp), "'"};
    }
  }

  std::error_code ec;
  std::filesystem::rename(target_tmp, target, ec);
  if (ec) {
    std::filesystem::remove(target_tmp, ec);
    throw LinpipeError{"write_atomically: Cannot create file '", path_to_utf8(target), "': ", ec.message()};
  }
}

bool is_valid_timestamp(const std::string& timestamp) {
  /* Checks that a timestamp has exactly the form YYYY-MM-DDTHH:MM:SSZ.

  Receives:
    timestamp: the timestamp to check

  Returns:
    true if the timestamp has the required form
  */

  static constexpr std::string_view pattern = "dddd-dd-ddTdd:dd:ddZ";
  if (timestamp.size() != pattern.size()) return false;
  for (size_t i = 0; i < pattern.size(); i++)
    if (pattern[i] == 'd' ? !(timestamp[i] >= '0' && timestamp[i] <= '9') : timestamp[i] != pattern[i])
      return false;
  return true;
}

Json parse_repo_json(std::string_view content, std::string_view source) {
  /* Parses the repository overview JSON.

  Receives:
    content: the JSON text
    source: file or URL the content comes from, used in error messages

  Returns:
    the parsed overview

  Throws:
    LinpipeError if the content is not a valid overview.
  */

  Json repo;
  try {
    repo = Json::parse(content);
  } catch (Json::exception& e) {
    throw LinpipeError{"parse_repo_json: The model overview from '", source, "' is not valid JSON: ", e.what()};
  }

  if (!repo.is_object())
    throw LinpipeError{"parse_repo_json: The model overview from '", source, "' is not a JSON object"};
  if (!repo.contains("timestamp") || !repo["timestamp"].is_string() || !is_valid_timestamp(repo["timestamp"].get<std::string>()))
    throw LinpipeError{"parse_repo_json: The model overview from '", source, "' has no valid timestamp in the YYYY-MM-DDTHH:MM:SSZ format"};
  if (!repo.contains("models") || !repo["models"].is_object())
    throw LinpipeError{"parse_repo_json: The model overview from '", source, "' has no 'models' object"};

  return repo;
}

bool is_safe_name(std::string_view name) {
  /* Checks that a model or file name can be safely used as a directory or file.

  Receives:
    name: the name to check

  Returns:
    true if the name can be used as a directory or file name
  */

  if (name.empty() || name[0] == '.') return false;
  for (char c : name)
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
          c == '-' || c == '_' || c == '.' || c == '+'))
      return false;
  return true;
}

Json parse_model_json(std::string_view content, std::string_view source, std::string_view name) {
  /* Parses the JSON of a single model.

  Receives:
    content: the JSON text
    source: file or URL the content comes from, used in error messages
    name: the expected model name

  Returns:
    the parsed model JSON

  Throws:
    LinpipeError if the content is not a valid JSON of the given model.
  */

  Json model;
  try {
    model = Json::parse(content);
  } catch (Json::exception& e) {
    throw LinpipeError{"parse_model_json: The model JSON from '", source, "' is not valid JSON: ", e.what()};
  }

  if (!model.is_object())
    throw LinpipeError{"parse_model_json: The model JSON from '", source, "' is not a JSON object"};
  if (!model.contains("name") || !model["name"].is_string() || model["name"].get<std::string>() != name)
    throw LinpipeError{"parse_model_json: The model JSON from '", source, "' does not have the expected name '", name, "'"};
  if (!model.contains("files") || !model["files"].is_array() || model["files"].empty())
    throw LinpipeError{"parse_model_json: The model JSON from '", source, "' has no non-empty 'files' array"};

  for (const auto& file : model["files"]) {
    if (!file.is_object() || !file.contains("name") || !file["name"].is_string())
      throw LinpipeError{"parse_model_json: The model JSON from '", source, "' has a file without a 'name'"};
    auto file_name = file["name"].get<std::string>();
    if (!is_safe_name(file_name) || file_name == ModelHub::model_json_name)
      throw LinpipeError{"parse_model_json: The model JSON from '", source, "' has a file name '", file_name, "' that cannot be used as a file name"};
    if (!file.contains("url") || !file["url"].is_string())
      throw LinpipeError{"parse_model_json: The model JSON from '", source, "' has no 'url' for the file '", file_name, "'"};
    if (!file.contains("size") || !file["size"].is_number_integer() || file["size"].get<int64_t>() < 0)
      throw LinpipeError{"parse_model_json: The model JSON from '", source, "' has no non-negative integer 'size' for the file '", file_name, "'"};
  }

  return model;
}

std::optional<std::string> read_file(const std::filesystem::path& path) {
  /* Reads the whole file into a string.

  Receives:
    path: path of the file to read

  Returns:
    the file content, or nullopt if the file cannot be read
  */

  std::ifstream is(path, std::ios::binary);
  if (!is) return std::nullopt;
  std::string content{std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()};
  if (is.bad()) return std::nullopt;
  return content;
}

} // namespace

ModelHub::ModelHub(const std::string& dir, const std::string& repo_url)
  : dir(dir.empty() ? default_dir() : dir),
    repo_url(repo_url.empty() ? std::string(default_repo_url) : repo_url) {
  /* Creates a model hub; no files are accessed until a model is requested.

  Receives:
    dir: local cache directory (UTF-8). If empty, a platform-specific default
      is used, see default_dir(). Default: empty.
    repo_url: URL of the repository overview JSON. If empty, default_repo_url
      is used. Default: empty.

  Throws:
    LinpipeError if dir is empty and the default directory cannot be determined.
  */
}

// Defined here, where Json is a complete type, as needed by std::unique_ptr<Json>.
ModelHub::~ModelHub() = default;

std::string ModelHub::default_dir() {
  /* Gets the platform-specific default cache directory:
    Linux: $XDG_CACHE_HOME/linpipe/model_hub, or ~/.cache/linpipe/model_hub
    Windows: %LOCALAPPDATA%\linpipe\model_hub
    macOS: ~/Library/Caches/linpipe/model_hub

  Returns:
    the default cache directory (UTF-8)

  Throws:
    LinpipeError if the directory cannot be determined from the environment.
  */

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
    throw LinpipeError("ModelHub::default_dir: Cannot determine the default ModelHub directory, please specify it explicitly");

  return path_to_utf8(base / "linpipe" / "model_hub");
}

Model* ModelHub::get_model(const std::string& name) {
  /* Gets the model with the given name.

  Receives:
    name: model name, a key of the 'models' object in the overview JSON

  Returns:
    pointer to the model; currently always nullptr, as loading is not
    implemented yet

  Throws:
    LinpipeError if no usable repository overview is available, or if the
      model JSON or the model files cannot be obtained, see
      ensure_local_model() and ensure_local_files().
  */

  LOG(INFO, "ModelHub: model '" << name << "' requested");
  ensure_local_repo();
  auto model = ensure_local_model(name);
  ensure_local_files(name, model);

  // TODO: Check the size and SHA-256 of the files, unpack them, and load the model.
  return nullptr;
}

void ModelHub::ensure_local_repo() {
  /* Loads the repository overview into repo, once per instance.

  Throws:
    LinpipeError if the cache directory cannot be created, or if neither
      a valid cached nor a valid remote overview is available.
  */

  if (repo) return;

  auto dir_path = path_from_utf8(dir);

  std::error_code ec;
  bool created = std::filesystem::create_directories(dir_path, ec);
  if (ec || !std::filesystem::is_directory(dir_path, ec))
    throw LinpipeError{"ModelHub::ensure_local_repo: Cannot create ModelHub directory '", dir, "'", ec ? ": " : "", ec ? ec.message() : ""};
  if (created)
    LOG(INFO, "ModelHub: created a new model hub in '" << dir << "'");

  // Load the cached version, if there is a valid one.
  auto repo_json = dir_path / repo_json_name;
  auto repo_json_utf8 = path_to_utf8(repo_json);
  std::optional<Json> local;
  if (std::filesystem::exists(repo_json, ec)) {
    if (auto content = read_file(repo_json)) {
      try {
        local = parse_repo_json(*content, repo_json_utf8);
      } catch (LinpipeError& e) {
        LOG(WARN, "ModelHub: ignoring the cached model overview: " << e.what());
      }
    } else {
      LOG(WARN, "ModelHub: ignoring the cached model overview, cannot read '" << repo_json_utf8 << "'");
    }
  }

  // Try obtaining the remote version.
  std::optional<Json> remote;
  std::string remote_content;
  try {
    remote_content = fetch(repo_url);
    remote = parse_repo_json(remote_content, repo_url);
  } catch (LinpipeError& e) {
    if (!local)
      throw LinpipeError{"ModelHub::ensure_local_repo: No usable model overview is available: ", e.what()};
    LOG(WARN, "ModelHub: cannot update the model overview, using the cached one: " << e.what());
  }

  // Keep the newer of the two, storing the remote one in the cache if it wins.
  if (remote && (!local || (*remote)["timestamp"].get<std::string>() > (*local)["timestamp"].get<std::string>())) {
    write_atomically(repo_json, remote_content);
    LOG(INFO, "ModelHub: " << (local ? "updated" : "downloaded") << " the model overview from '" << repo_url << "'");
    local = std::move(remote);
  }

  repo = std::make_unique<Json>(std::move(*local));
  LOG(INFO, "ModelHub: loaded the model overview with timestamp " << (*repo)["timestamp"].get<std::string>()
      << " and " << (*repo)["models"].size() << " model(s)");
}

Json ModelHub::ensure_local_model(const std::string& name) {
  /* Makes sure the directory of the given model contains its model JSON.

  Receives:
    name: model name, a key of the 'models' object in the overview JSON

  Returns:
    the parsed model JSON

  Throws:
    LinpipeError if the model is not in the repository overview, its name
      cannot be used as a directory name, it has no URL, or its model JSON
      cannot be downloaded, is not valid, or cannot be stored.
  */

  const auto& models = (*repo)["models"];
  if (!models.contains(name))
    throw LinpipeError{"ModelHub::ensure_local_model: Model '", name, "' was not found in the model overview"};
  if (!is_safe_name(name))
    throw LinpipeError{"ModelHub::ensure_local_model: Model name '", name, "' cannot be used as a directory name"};
  const auto& entry = models[name];
  if (!entry.is_object() || !entry.contains("url") || !entry["url"].is_string())
    throw LinpipeError{"ModelHub::ensure_local_model: Model '", name, "' has no 'url' in the model overview"};
  auto url = entry["url"].get<std::string>();

  auto model_dir = path_from_utf8(dir) / path_from_utf8(name);
  auto model_json = model_dir / model_json_name;
  auto model_json_utf8 = path_to_utf8(model_json);

  // Use the cached model JSON, if there is a valid one.
  std::error_code ec;
  if (std::filesystem::exists(model_json, ec)) {
    if (auto content = read_file(model_json)) {
      try {
        return parse_model_json(*content, model_json_utf8, name);
      } catch (LinpipeError& e) {
        LOG(WARN, "ModelHub: ignoring the cached model JSON: " << e.what());
      }
    } else {
      LOG(WARN, "ModelHub: ignoring the cached model JSON, cannot read '" << model_json_utf8 << "'");
    }
  }

  // Download and validate the model JSON before creating anything on disk.
  std::string content;
  Json model;
  try {
    content = fetch(url);
    model = parse_model_json(content, url, name);
  } catch (LinpipeError& e) {
    throw LinpipeError{"ModelHub::ensure_local_model: Cannot obtain the JSON of model '", name, "': ", e.what()};
  }

  std::filesystem::create_directories(model_dir, ec);
  if (ec || !std::filesystem::is_directory(model_dir, ec))
    throw LinpipeError{"ModelHub::ensure_local_model: Cannot create model directory '", path_to_utf8(model_dir), "'", ec ? ": " : "", ec ? ec.message() : ""};

  write_atomically(model_json, content);
  LOG(INFO, "ModelHub: downloaded the JSON of model '" << name << "' to '" << model_json_utf8 << "'");
  return model;
}

void ModelHub::ensure_local_files(const std::string& name, const Json& model) {
  /* Makes sure the directory of the given model contains all its files.

  Receives:
    name: model name, whose directory already exists
    model: the parsed model JSON, see parse_model_json()

  Throws:
    LinpipeError if a file cannot be downloaded, does not have the expected
      size, or cannot be stored.
  */

  auto model_dir = path_from_utf8(dir) / path_from_utf8(name);

  for (const auto& file : model["files"]) {
    auto file_name = file["name"].get<std::string>();
    auto file_path = model_dir / path_from_utf8(file_name);

    auto size = file["size"].get<uint64_t>();

    std::error_code ec;
    if (std::filesystem::exists(file_path, ec)) {
      auto local_size = std::filesystem::file_size(file_path, ec);
      if (!ec && local_size == size)
        continue;
      LOG(WARN, "ModelHub: the file '" << file_name << "' of model '" << name << "' has size "
          << (ec ? "unknown" : std::to_string(local_size)) << " instead of " << size << ", downloading it again");
    }

    auto url = file["url"].get<std::string>();
    std::string content;
    try {
      content = fetch(url);
    } catch (LinpipeError& e) {
      throw LinpipeError{"ModelHub::ensure_local_files: Cannot obtain the file '", file_name, "' of model '", name, "': ", e.what()};
    }
    if (content.size() != size)
      throw LinpipeError{"ModelHub::ensure_local_files: The file '", file_name, "' of model '", name, "' downloaded from '", url,
                         "' has size ", std::to_string(content.size()), " instead of the expected ", std::to_string(size)};

    write_atomically(file_path, content);
    LOG(INFO, "ModelHub: downloaded the file '" << file_name << "' of model '" << name << "'");
  }
}

} // namespace linpipe
