// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "models/model.h"
#include "utils/asset_downloader.h"

namespace linpipe {

class ModelHub {
 public:
  explicit ModelHub(std::string repo_url = {});

  std::unique_ptr<Model> get_model(std::string_view name);

 private:
  static constexpr std::string_view default_repo_url_ = "https://huggingface.com/ufal/linpipe/repo/models";
  static constexpr std::string_view family_ = "model";

  AssetDownloader asset_downloader_;
};

} // namespace linpipe
