// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <utility>

#include "lib/json/json.h"
#include "models/model_factory.h"
#include "models/model_hub.h"
#include "utils/json_utils.h"

namespace linpipe {

namespace {

std::optional<std::string_view> family_language(std::string_view name) {
  auto family_end = name.find('-');
  if (family_end == 0 || family_end == std::string_view::npos)
    return std::nullopt;
  auto language_end = name.find('-', family_end + 1);
  if (language_end == family_end + 1 || language_end == std::string_view::npos)
    return std::nullopt;
  return name.substr(0, language_end);
}

} // namespace

ModelHub::ModelHub(std::string repo_url) : asset_downloader_(repo_url.empty() ? std::string(default_repo_url_) : std::move(repo_url), std::string(family_)) {}

std::unique_ptr<Model> ModelHub::get_model(std::string_view name) {
  constexpr std::string_view caller = "ModelHub::get_model";

  auto model_dir = locate_model(name);

  Json config = json_parse_path(caller, model_dir / "model.json");
  return create_model(name, model_dir, config);
}

std::filesystem::path ModelHub::locate_model(std::string_view name) {
  if (auto model_dir = asset_downloader_.get_downloaded_asset(name))
    return *model_dir;

  if (auto it = catalog().find(std::string(name)); it != catalog().end())
    return asset_downloader_.get_asset(it->second);

  return asset_downloader_.get_asset(name);
}

const std::unordered_map<std::string, std::string>& ModelHub::catalog() {
  std::call_once(catalog_once_, [this] {
    constexpr std::string_view caller = "ModelHub::catalog";

    // "family-language" -> (timestamp, name) of the newest model so far.
    std::unordered_map<std::string, std::pair<std::string, std::string>> newest;
    for (const auto& entry : json_get_object(caller, asset_downloader_.assets_json(), "entries").items()) {
      auto key = family_language(entry.key());
      if (!key)
        continue;

      std::pair<std::string, std::string> candidate{json_get<std::string>(caller, entry.value(), "timestamp"), entry.key()};
      auto [it, inserted] = newest.try_emplace(std::string(*key), candidate);
      if (!inserted && it->second < candidate)
        it->second = std::move(candidate);
    }

    std::unordered_map<std::string, std::string> catalog;
    for (auto& [key, timestamp_name] : newest)
      catalog.emplace(key, std::move(timestamp_name.second));
    catalog_ = std::move(catalog);
  });

  return catalog_;
}

} // namespace linpipe
