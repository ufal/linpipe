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
    EnvGuard models_dir("LINPIPE_MODELS_DIR", path_to_utf8(dir.path()).c_str());
    CHECK(AssetDownloader(TEST_MODELS_JSON_URL, "model").cache_dir() == dir.path());
  }

  SUBCASE("LINPIPE_CACHE_DIR") {
    EnvGuard models_dir("LINPIPE_MODELS_DIR", "");
    EnvGuard cache_dir("LINPIPE_CACHE_DIR", path_to_utf8(dir.path()).c_str());
    CHECK(AssetDownloader(TEST_MODELS_JSON_URL, "model").cache_dir() == dir.path() / "models");
  }
}

TEST_CASE("AssetDownloader downloads and verifies a model" * doctest::may_fail(true)) {
  TempDir dir("linpipe_asset_downloader_test");
  EnvGuard models_dir("LINPIPE_MODELS_DIR", path_to_utf8(dir.path()).c_str());
  EnvGuard offline("LINPIPE_OFFLINE", "");

  AssetDownloader downloader(TEST_MODELS_JSON_URL, "model");
  auto model_dir = dir.path() / TEST_MODEL;

  // Nothing is downloaded yet.
  CHECK(!downloader.get_downloaded_asset(TEST_MODEL));

  // The single download: the model index and the model.
  REQUIRE(downloader.get_asset(TEST_MODEL) == model_dir);

  // The model index.
  CHECK(std::filesystem::is_regular_file(dir.path() / "models.json"));
  CHECK(downloader.assets_json()["entries"].contains(TEST_MODEL));

  // The model.
  CHECK(read_file(model_dir / "README.md") == TEST_README_CONTENT);
  CHECK(read_file(model_dir / "test_model.txt") == TEST_MODEL_TXT_CONTENT);
  CHECK(downloader.get_downloaded_asset(TEST_MODEL) == model_dir);

  // Damage the downloaded model.
  Json model_json = Json::parse(read_file(model_dir / "model.json"));
  bool readme_found = false;
  for (auto& file : model_json["files"])
    if (file["name"] == "README.md") {
      file["sha256"] = std::string(64, '0');
      readme_found = true;
    }
  REQUIRE(readme_found);
  std::ofstream(model_dir / "model.json") << model_json.dump(2);
  std::filesystem::remove(model_dir / "README.md");

  CHECK_THROWS_WITH_AS(downloader.get_asset(TEST_MODEL), doctest::Contains("SHA-256"), LinpipeError);
  CHECK(!std::filesystem::exists(model_dir / "README.md"));
}

} // namespace linpipe
