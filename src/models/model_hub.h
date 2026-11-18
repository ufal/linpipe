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
#include <map>
#include <mutex>

#include "common.h"
#include "models/model.h"
#include "utils/asset_downloader.h"
#include "utils/str_icase.h"

namespace linpipe {

class ModelHub {
 public:
  explicit ModelHub(std::string models_json_url = {})
      : asset_downloader_(models_json_url.empty() ? std::string(default_models_json_url_) : std::move(models_json_url), std::string(family_)) {}

  // A verbatim model name, "family-language", or "family"; the family is
  // case-insensitive and the language can be any ISO 639 code or English name.
  std::unique_ptr<Model> get_model(std::string_view name);
  std::filesystem::path locate_model(std::string_view name);

 private:
  struct CatalogKeyLess {
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const { return str_ascii_icase_lt(a, b); }
  };
  using Catalog = std::map<std::string, std::string, CatalogKeyLess>;

  // Lazily built from models.json: "family-language" and "family" -> newest model name,
  // with the language canonicalized and the keys compared case-insensitively.
  const Catalog& catalog();

  // TODO: Replace with real model repo once created
  static constexpr std::string_view default_models_json_url_ = "https://huggingface.co/ufal/linpipe/resolve/main/models.json";
  static constexpr std::string_view family_ = "model";

  AssetDownloader asset_downloader_;

  Catalog catalog_;
  std::once_flag catalog_once_;
};

} // namespace linpipe
