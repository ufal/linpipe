// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <array>
#include <utility>

#include "lib/json/json.h"
#include "models/model_factory.h"
#include "models/model_hub.h"
#include "utils/json_utils.h"
#include "utils/languages.h"
#include "utils/path_utf8.h"

namespace linpipe {

namespace {

std::string family_language_key(std::string_view family, std::string_view language) {
  return fmt::format("{}-{}", family, Languages::canonicalize_code_or_name(language));
}

// The "family" and "family-language" catalog keys of a "family-language-version" model name.
std::optional<std::array<std::string, 2>> model_catalog_keys(std::string_view name) {
  auto family_end = name.find('-');
  if (family_end == 0 || family_end == std::string_view::npos)
    return std::nullopt;
  auto language_end = name.find('-', family_end + 1);
  if (language_end == family_end + 1 || language_end == std::string_view::npos)
    return std::nullopt;

  auto family = name.substr(0, family_end);
  auto language = name.substr(family_end + 1, language_end - family_end - 1);
  return std::array{std::string(family), family_language_key(family, language)};
}

// The catalog key of a requested "family" or "family-language"; the language
// is everything after the first dash, because language names may contain dashes.
std::string requested_catalog_key(std::string_view name) {
  auto family_end = name.find('-');
  if (family_end == std::string_view::npos)
    return std::string(name);
  return family_language_key(name.substr(0, family_end), name.substr(family_end + 1));
}

} // namespace

std::unique_ptr<Model> ModelHub::get_model(std::string_view name) {
  auto model_dir = locate_model(name);
  auto model_name = path_to_utf8(model_dir.filename());

  Json config = json_parse_path(model_dir / "model.json");
  return create_model(model_name, model_dir, config);
}

std::filesystem::path ModelHub::locate_model(std::string_view name) {
  // A verbatim model name, either already downloaded, or present in models.json.
  if (auto model_dir = asset_downloader_.get_downloaded_asset(name))
    return *model_dir;

  if (json_get_object(asset_downloader_.assets_json(), "entries").contains(name))
    return asset_downloader_.get_asset(name);

  // A "family-language" or a "family".
  if (auto it = catalog().find(requested_catalog_key(name)); it != catalog().end()) {
    LOG(INFO, "Resolved model {} to {}", name, it->second);
    return asset_downloader_.get_asset(it->second);
  }

  // The catalog is ordered, so the families are listed sorted.
  std::vector<std::string_view> families;
  for (const auto& [key, model] : catalog())
    if (key.find('-') == std::string::npos)
      families.push_back(key);

  throw LinPipeError{"Unknown model {}, which is neither a model name, nor a 'family-language', nor a 'family'; the known families are: {}", name, fmt::join(families, ", ")};
}

const ModelHub::Catalog& ModelHub::catalog() {
  std::call_once(catalog_once_, [this] {
    // "family" or "family-language" -> (timestamp, name) of the newest model so far.
    std::map<std::string, std::pair<std::string, std::string>, CatalogKeyLess> newest;
    for (const auto& entry : json_get_object(asset_downloader_.assets_json(), "entries").items()) {
      auto keys = model_catalog_keys(entry.key());
      if (!keys)
        continue;

      std::pair<std::string, std::string> candidate{json_get<std::string>(entry.value(), "timestamp"), entry.key()};
      for (auto& key : *keys) {
        auto [it, inserted] = newest.try_emplace(std::move(key), candidate);
        if (!inserted && it->second < candidate)
          it->second = candidate;
      }
    }

    for (auto& [key, timestamp_name] : newest)
      catalog_.emplace(key, std::move(timestamp_name.second));
  });

  return catalog_;
}

} // namespace linpipe
