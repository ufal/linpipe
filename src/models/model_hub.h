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
#include "core/selector.h"
#include "lib/json/json.h"
#include "models/model.h"
#include "utils/asset_downloader.h"

namespace linpipe {

class ModelHub {
 public:
  explicit ModelHub(std::string models_json_url = {})
      : asset_downloader_(models_json_url.empty() ? std::string(default_models_json_url_) : std::move(models_json_url), std::string(family_)) {}

  // Either a verbatim model name, or colon-separated selectors, from which the newest matching model is chosen:
  // - `name` or unkeyed: the model name, or its prefix before the first dash, compared case-insensitively;
  //   an unkeyed `prefix-language` is a shorthand for `prefix:language=language`;
  // - `language`: one of the model languages, given as any ISO 639 code or English name;
  // - `operation`: one of the model operations.
  // For example, "test_model", "test_model-en", or "test_model:language=English".
  std::unique_ptr<Model> get_model(std::string_view name);
  std::filesystem::path locate_model(std::string_view name);

 private:
  static bool matches_selector(std::string_view model_name, const Json& entry, const Selector& selector);

  // TODO: Replace with real model repo once created
  static constexpr std::string_view default_models_json_url_ = "https://huggingface.co/ufal/linpipe/resolve/main/models.json";
  static constexpr std::string_view family_ = "model";

  AssetDownloader asset_downloader_;
};

} // namespace linpipe
