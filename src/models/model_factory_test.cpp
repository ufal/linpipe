// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>

#include "lib/doctest/doctest.h"
#include "models/model_factory.h"
#include "models/test_model.h"
#include "models/test_model_wo_assets.h"
#include "utils/json_utils.h"
#include "utils/test_utils.h"

namespace linpipe {

TEST_CASE("create_model creates a model with assets from its dir and config") {
  LoggingLevelGuard logging(LoggingLevel::LEVEL_FATAL);
  TempDir dir("linpipe_model_factory_test");
  auto model_dir = dir.path() / TEST_MODEL;
  std::filesystem::create_directory(model_dir);
  {
    std::ofstream file(model_dir / "test_model.txt", std::ios::binary);
    file << TEST_MODEL_TXT_CONTENT;
  }

  Json config = {{"key", "value"}};
  auto model = create_model("TestModel", model_dir, std::move(config), TEST_MODEL);
  auto* test_model = dynamic_cast<TestModel*>(model.get());
  REQUIRE(test_model);
  CHECK(test_model->dir() == model_dir);
  CHECK(json_get<const std::string&>(test_model->config(), "key") == "value");
  CHECK(test_model->content() == TEST_MODEL_TXT_CONTENT);
}

TEST_CASE("create_model creates a model without assets by class name alone") {
  LoggingLevelGuard logging(LoggingLevel::LEVEL_FATAL);

  auto model = create_model("TestModelWOAssets");
  auto* test_model = dynamic_cast<TestModelWOAssets*>(model.get());
  REQUIRE(test_model);
  CHECK(test_model->dir().empty());
  CHECK(test_model->config().is_null());
  CHECK(test_model->content() == TestModelWOAssets::CONTENT);
}

TEST_CASE("create_model rejects an unknown class") {
  CHECK_THROWS_WITH_AS(create_model("NonexistentModel"), doctest::Contains("the class is unknown"), LinPipeError);
  CHECK_THROWS_WITH_AS(create_model("NonexistentModel", {}, {}, "my_model"),
                       doctest::Contains("Model my_model of class NonexistentModel"), LinPipeError);
}

} // namespace linpipe
