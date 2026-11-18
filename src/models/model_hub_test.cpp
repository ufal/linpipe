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

TEST_CASE("ModelHub locates and loads models" * doctest::may_fail(true)) {
  LoggingLevelGuard logging(LoggingLevel::LEVEL_FATAL);
  TempDir dir("linpipe_model_hub_test");
  EnvGuard models_dir("LINPIPE_MODELS_DIR", dir.path());
  EnvGuard offline("LINPIPE_OFFLINE", nullptr);

  ModelHub hub(TEST_MODELS_JSON_URL);
  auto model_dir = dir.path() / TEST_MODEL;

  // Unmatched selectors need only the model index, and list the known model names.
  CHECK_THROWS_WITH_AS(hub.locate_model("nonexistent"), doctest::Contains("test_model"), LinPipeError);
  CHECK_THROWS_WITH_AS(hub.locate_model("test_model-de"), doctest::Contains("test_model"), LinPipeError);
  CHECK_THROWS_WITH_AS(hub.locate_model("test_model:operation=other"), doctest::Contains("test_model"), LinPipeError);
  CHECK_THROWS_WITH_AS(hub.locate_model("test_model:unknown=key"), doctest::Contains("unknown"), LinPipeError);
  CHECK(!std::filesystem::exists(model_dir));

  {
    // An offline hub sees the index, but cannot download the model.
    EnvGuard offline("LINPIPE_OFFLINE", "1");
    CHECK_THROWS_WITH_AS(ModelHub(TEST_MODELS_JSON_URL).locate_model("test_model"), doctest::Contains("offline"), LinPipeError);
  }

  // The single download, by the verbatim model name.
  CHECK(hub.locate_model(TEST_MODEL) == model_dir);

  // Already downloaded: the verbatim name, the "name-language" shorthand, and selectors.
  CHECK(hub.locate_model(TEST_MODEL) == model_dir);
  CHECK(hub.locate_model("test_model-en") == model_dir);
  CHECK(hub.locate_model("test_model") == model_dir);
  for (const auto* name : {"test_model:language=en", "name=test_model", "language=en:operation=test_operation", "family=TestModel"})
    CHECK(hub.locate_model(name) == model_dir);

  // The name is case-insensitive, the language is any code or name.
  for (const auto* name : {"TEST_MODEL", "Test_Model-EN", "test_model-eng", "test_model-English", "test_model:language=ENGLISH"})
    CHECK(hub.locate_model(name) == model_dir);

  {
    // An offline hub finds the downloaded model.
    EnvGuard offline("LINPIPE_OFFLINE", "1");
    CHECK(ModelHub(TEST_MODELS_JSON_URL).locate_model("test_model-en") == model_dir);
  }

  // Loading the model reads its files.
  auto model = hub.get_model("test_model");
  auto* test_model = dynamic_cast<TestModel*>(model.get());
  REQUIRE(test_model);
  CHECK(test_model->dir() == model_dir);
  CHECK(test_model->content() == TEST_MODEL_TXT_CONTENT);
}

} // namespace linpipe
