// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <utility>

#include "lib/json/json.h"
#include "models/model_factory.h"
#include "models/model_hub.h"
#include "utils/json_utils.h"
#include "utils/path_utf8.h"

namespace linpipe {

namespace {

std::optional<std::array<std::string_view, 2>> catalog_keys(std::string_view name) {
  auto family_end = name.find('-');
  if (family_end == 0 || family_end == std::string_view::npos)
    return std::nullopt;
  auto language_end = name.find('-', family_end + 1);
  if (language_end == family_end + 1 || language_end == std::string_view::npos)
    return std::nullopt;
  return std::array{name.substr(0, family_end), name.substr(0, language_end)};
}

} // namespace

ModelHub::ModelHub(std::string models_json_url) : asset_downloader_(models_json_url.empty() ? std::string(default_models_json_url_) : std::move(models_json_url), std::string(family_)) {}

std::unique_ptr<Model> ModelHub::get_model(std::string_view name) {
  auto model_dir = locate_model(name);
  auto model_name = path_to_utf8(model_dir.filename());

  Json config = json_parse_path(model_dir / "model.json");
  return create_model(model_name, model_dir, config);
}

std::filesystem::path ModelHub::locate_model(std::string_view name) {
  if (auto model_dir = asset_downloader_.get_downloaded_asset(name))
    return *model_dir;

  if (auto it = catalog().find(std::string(name)); it != catalog().end()) {
    LOG(INFO, "Resolved model {} to {}", name, it->second);
    return asset_downloader_.get_asset(it->second);
  }

  if (!json_get_object(asset_downloader_.assets_json(), "entries").contains(name)) {
    std::vector<std::string_view> families;
    for (const auto& [key, model] : catalog())
      if (key.find('-') == std::string::npos)
        families.push_back(key);
    std::ranges::sort(families);

    throw LinpipeError{"Unknown model {}, which is neither a model name, nor a 'family-language', nor a 'family'; the known families are: {}", name, fmt::format("{}", fmt::join(families, ", "))};
  }

  return asset_downloader_.get_asset(name);
}

const std::unordered_map<std::string, std::string>& ModelHub::catalog() {
  std::call_once(catalog_once_, [this] {
    // "family" or "family-language" -> (timestamp, name) of the newest model so far.
    std::unordered_map<std::string, std::pair<std::string, std::string>> newest;
    for (const auto& entry : json_get_object(asset_downloader_.assets_json(), "entries").items()) {
      auto keys = catalog_keys(entry.key());
      if (!keys)
        continue;

      std::pair<std::string, std::string> candidate{json_get<std::string>(entry.value(), "timestamp"), entry.key()};
      for (auto key : *keys) {
        auto [it, inserted] = newest.try_emplace(std::string(key), candidate);
        if (!inserted && it->second < candidate)
          it->second = candidate;
      }
    }

    std::unordered_map<std::string, std::string> catalog;
    for (auto& [key, timestamp_name] : newest)
      catalog.emplace(key, std::move(timestamp_name.second));
    catalog_ = std::move(catalog);
  });

  return catalog_;
}

} // namespace linpipe
