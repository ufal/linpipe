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

constexpr const char* TEST_MODELS_JSON_URL = "https://raw.githubusercontent.com/ufal/linpipe/master/tests/test_model_repo/models.json";
constexpr const char* TEST_MODEL = "test_model-en-261005";

constexpr const char* TEST_README_CONTENT = "# LinPipe Test Model\n\nThis is a test model for LinPipe.\n";
constexpr const char* TEST_MODEL_TXT_CONTENT = "This is a test model for LinPipe.\n";

std::string read_file(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);
  REQUIRE(file);
  std::ostringstream content;
  content << file.rdbuf();
  return content.str();
}

} // namespace

TEST_CASE("AssetDownloader cache_dir") {
  TempDir dir;

  SUBCASE("LINPIPE_MODELS_DIR") {
    EnvGuard models_dir("LINPIPE_MODELS_DIR", dir.path());
    CHECK(AssetDownloader(TEST_MODELS_JSON_URL, "model").cache_dir() == dir.path());
  }

  SUBCASE("LINPIPE_CACHE_DIR") {
    EnvGuard models_dir("LINPIPE_MODELS_DIR", nullptr);
    EnvGuard cache_dir("LINPIPE_CACHE_DIR", dir.path());
    CHECK(AssetDownloader(TEST_MODELS_JSON_URL, "model").cache_dir() == dir.path() / "models");
  }
}

TEST_CASE("AssetDownloader downloads and verifies a model" * doctest::may_fail(true)) {
  LoggingLevelGuard logging(LoggingLevel::LEVEL_FATAL);
  TempDir dir("linpipe_asset_downloader_test");
  EnvGuard models_dir("LINPIPE_MODELS_DIR", dir.path());
  EnvGuard offline("LINPIPE_OFFLINE", nullptr);

  AssetDownloader downloader(TEST_MODELS_JSON_URL, "model");
  auto model_dir = dir.path() / TEST_MODEL;

  // Download only the model index.
  CHECK(downloader.assets_json()["entries"].contains(TEST_MODEL));
  CHECK(!downloader.get_downloaded_asset(TEST_MODEL));

  {
    // An offline downloader sees the index, but cannot download the model.
    EnvGuard offline("LINPIPE_OFFLINE", "1");
    AssetDownloader offline_downloader(TEST_MODELS_JSON_URL, "model");
    CHECK(offline_downloader.assets_json()["entries"].contains(TEST_MODEL));
    CHECK_THROWS_WITH_AS(offline_downloader.get_asset(TEST_MODEL), doctest::Contains("offline"), LinPipeError);
  }

  // Download the model.
  REQUIRE(downloader.get_asset(TEST_MODEL) == model_dir);
  CHECK(read_file(model_dir / "README.md") == TEST_README_CONTENT);
  CHECK(read_file(model_dir / "test_model.txt") == TEST_MODEL_TXT_CONTENT);
  CHECK(downloader.get_downloaded_asset(TEST_MODEL) == model_dir);

  // An appended character is detected by the size check.
  std::ofstream(model_dir / "README.md", std::ios::binary | std::ios::app) << '!';
  CHECK(!downloader.get_downloaded_asset(TEST_MODEL));
  {
    EnvGuard offline("LINPIPE_OFFLINE", "1");
    CHECK_THROWS_WITH_AS(AssetDownloader(TEST_MODELS_JSON_URL, "model").get_asset(TEST_MODEL), doctest::Contains("offline"), LinPipeError);
  }

  // With a wrong SHA-256 in model.json, the re-download of a missing README.md fails.
  auto model_json = read_file(model_dir / "model.json");
  Json damaged_json = Json::parse(model_json);
  for (auto& file : damaged_json["files"])
    if (file["name"] == "README.md")
      file["sha256"] = std::string(64, '0');
  std::ofstream(model_dir / "model.json", std::ios::binary) << damaged_json.dump();
  std::filesystem::remove(model_dir / "README.md");
  CHECK_THROWS_WITH_AS(downloader.get_asset(TEST_MODEL), doctest::Contains("SHA-256"), LinPipeError);
  CHECK(!std::filesystem::exists(model_dir / "README.md"));
}

} // namespace linpipe
