// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include <sstream>

#include "lib/doctest/doctest.h"
#include "utils/asset_downloader.h"
#include "utils/test_utils.h"

namespace linpipe {

namespace {

constexpr const char* TEST_REPO_URL = "https://raw.githubusercontent.com/ufal/linpipe/master/tests/test_model_repo";
constexpr const char* TEST_MODELS_JSON_URL = "https://raw.githubusercontent.com/ufal/linpipe/master/tests/test_model_repo/models.json";
constexpr const char* TEST_MODEL = "test_model-en-261005";

constexpr const char* TEST_README_CONTENT = "# Linpipe Test Model\n\nThis is a test model for LinPipe.\n";
constexpr const char* TEST_MODEL_TXT_CONTENT = "This is a test model for LinPipe.\n";

std::string read_file(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);
  REQUIRE(file);
  std::ostringstream content;
  content << file.rdbuf();
  return content.str();
}

struct CleanEnv {
  EnvGuard models_dir{"LINPIPE_MODELS_DIR", ""};
  EnvGuard cache_dir{"LINPIPE_CACHE_DIR", ""};
  EnvGuard offline{"LINPIPE_OFFLINE", ""};
};

struct ModelCache {
  CleanEnv clean_env;
  TempDir dir{"linpipe_asset_downloader_test"};
  EnvGuard models_dir{"LINPIPE_MODELS_DIR", path_to_utf8(dir.path()).c_str()};
};

} // namespace

TEST_CASE("AssetDownloader cache_dir") {
  CleanEnv env;
  TempDir dir;

  SUBCASE("LINPIPE_MODELS_DIR") {
    EnvGuard models_dir("LINPIPE_MODELS_DIR", path_to_utf8(dir.path()).c_str());
    CHECK(AssetDownloader(TEST_MODELS_JSON_URL, "model").cache_dir() == dir.path());
  }

  SUBCASE("LINPIPE_CACHE_DIR") {
    EnvGuard cache_dir("LINPIPE_CACHE_DIR", path_to_utf8(dir.path()).c_str());
    CHECK(AssetDownloader(TEST_MODELS_JSON_URL, "model").cache_dir() == dir.path() / "models");
  }
}

TEST_CASE("AssetDownloader downloads the model index" * doctest::may_fail(true)) {
  ModelCache cache;
  AssetDownloader downloader(TEST_MODELS_JSON_URL, "model");

  CHECK(downloader.assets_json()["entries"].contains(TEST_MODEL));
  CHECK(std::filesystem::is_regular_file(cache.dir.path() / "models.json"));
}

TEST_CASE("AssetDownloader downloads a model" * doctest::may_fail(true)) {
  ModelCache cache;
  AssetDownloader downloader(TEST_MODELS_JSON_URL, "model");

  CHECK(!downloader.get_downloaded_asset(TEST_MODEL));
  auto model_dir = downloader.get_asset(TEST_MODEL);
  CHECK(model_dir == cache.dir.path() / TEST_MODEL);
  CHECK(read_file(model_dir / "README.md") == TEST_README_CONTENT);
  CHECK(read_file(model_dir / "test_model.txt") == TEST_MODEL_TXT_CONTENT);
  CHECK(downloader.get_downloaded_asset(TEST_MODEL) == model_dir);
}

TEST_CASE("AssetDownloader rejects a file with a wrong SHA-256" * doctest::may_fail(true)) {
  ModelCache cache;
  auto model_dir = cache.dir.path() / TEST_MODEL;

  // A schema-valid model.json whose single file has a wrong checksum.
  Json json = {{"timestamp", "2026-10-05T12:00:00Z"}, {"name", TEST_MODEL}, {"files", {{{"name", "README.md"}, {"url", fmt::format("{}/{}/README.md", TEST_REPO_URL, TEST_MODEL)}, {"sha256", std::string(64, '0')}, {"size", std::string_view(TEST_README_CONTENT).size()}}}}};
  std::filesystem::create_directories(model_dir);
  std::ofstream(model_dir / "model.json") << json.dump(2);

  AssetDownloader downloader(TEST_MODELS_JSON_URL, "model");
  CHECK_THROWS_WITH_AS(downloader.get_asset(TEST_MODEL), doctest::Contains("SHA-256"), LinpipeError);
  CHECK(!std::filesystem::exists(model_dir / "README.md"));
}

} // namespace linpipe
