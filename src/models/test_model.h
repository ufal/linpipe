// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <filesystem>

#include "common.h"
#include "lib/json/json.h"
#include "models/asset_model.h"

namespace linpipe {

class TestModel : public AssetModel {
 public:
  TestModel(const std::filesystem::path& dir, Json& config);

  const std::string& content() const { return content_; }

 private:
  std::string content_;
};

} // namespace linpipe
