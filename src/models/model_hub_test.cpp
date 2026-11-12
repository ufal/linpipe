// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest/doctest.h"
#include "models/model_hub.h"
#include "models/test_model.h"
#include "utils/test_utils.h"

namespace linpipe {

namespace {

constexpr const char* TEST_MODELS_JSON_URL = "https://raw.githubusercontent.com/ufal/linpipe/master/tests/test_model_repo/models.json";
constexpr const char* TEST_MODEL = "test_model-en-261005";

} // namespace

TEST_CASE("ModelHub locates and loads models" * doctest::may_fail(true)) {
  TempDir dir("linpipe_model_hub_test");
  EnvGuard models_dir("LINPIPE_MODELS_DIR", path_to_utf8(dir.path()).c_str());
  EnvGuard offline("LINPIPE_OFFLINE", "");

  ModelHub hub(TEST_MODELS_JSON_URL);
  auto model_dir = dir.path() / TEST_MODEL;

  // An unknown name needs only the model index, and lists the known families.
  CHECK_THROWS_WITH_AS(hub.locate_model("nonexistent"), doctest::Contains("test_model"), LinpipeError);
  CHECK(!std::filesystem::exists(model_dir));

  // The single download, by the verbatim model name.
  CHECK(hub.locate_model(TEST_MODEL) == model_dir);

  // Already downloaded: the verbatim name, "family-language" and "family".
  CHECK(hub.locate_model(TEST_MODEL) == model_dir);
  CHECK(hub.locate_model("test_model-en") == model_dir);
  CHECK(hub.locate_model("test_model") == model_dir);

  // Loading the model reads its files.
  auto model = hub.get_model("test_model");
  auto* test_model = dynamic_cast<TestModel*>(model.get());
  REQUIRE(test_model);
  CHECK(test_model->dir() == model_dir);
  CHECK(test_model->content() == "This is a test model for LinPipe.\n");
}

} // namespace linpipe
