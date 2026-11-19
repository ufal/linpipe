// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "models/model_factory.h"
#include "models/model_hub.h"
#include "utils/json_utils.h"
#include "utils/path_utf8.h"
#include "utils/str_icase.h"

namespace linpipe {

namespace {

std::string_view model_name_prefix(std::string_view model_name) {
  return model_name.substr(0, model_name.find('-'));
}

// Expands an unkeyed `prefix-language` into `prefix` and `language=language`;
// the language is everything after the first dash, because language names may contain dashes.
std::vector<Selector> parse_model_selectors(std::string_view input) {
  std::vector<Selector> selectors;
  for (auto& selector : parse_selectors(input)) {
    auto dash = selector.value.find('-');
    if (selector.key.empty() && dash != std::string::npos) {
      selectors.push_back({"", selector.value.substr(0, dash)});
      selectors.push_back({"language", selector.value.substr(dash + 1)});
    } else {
      selectors.push_back(std::move(selector));
    }
  }
  return selectors;
}

bool any_element_matches(const Json& entry, std::string_view key, const Selector& selector) {
  if (!entry.contains(key))
    return false;
  return std::ranges::any_of(json_get_array(entry, key), [&selector](const Json& element) {
    return selector.matches(json_get<const std::string&>(element));
  });
}

} // namespace

std::unique_ptr<Model> ModelHub::get_model(std::string_view name) {
  auto model_dir = locate_model(name);
  auto model_name = path_to_utf8(model_dir.filename());

  Json config = json_parse_path(model_dir / "model.json");
  return create_model(model_name, model_dir, config);
}

std::filesystem::path ModelHub::locate_model(std::string_view name) {
  const auto& entries = json_get_object(asset_downloader_.assets_json(), "entries");

  // A verbatim model name, either already downloaded, or present in models.json.
  if (name.find_first_of(":=") == std::string_view::npos) {
    if (auto model_dir = asset_downloader_.get_downloaded_asset(name))
      return *model_dir;
    if (entries.contains(name))
      return asset_downloader_.get_asset(name);
  }

  // Otherwise the newest model matching all the selectors.
  auto selectors = parse_model_selectors(name);
  if (selectors.empty())
    throw LinPipeError{"No model selectors were given"};

  const std::string* best_name = nullptr;
  std::string best_timestamp;
  for (const auto& entry : entries.items()) {
    auto matches = std::ranges::all_of(selectors, [&entry](const Selector& selector) {
      return matches_selector(entry.key(), entry.value(), selector);
    });
    if (!matches)
      continue;

    auto timestamp = json_get<std::string>(entry.value(), "timestamp");
    if (!best_name || best_timestamp < timestamp) {
      best_name = &entry.key();
      best_timestamp = std::move(timestamp);
    }
  }

  if (best_name) {
    LOG(INFO, "Resolved model {} to {}", name, *best_name);
    return asset_downloader_.get_asset(*best_name);
  }

  std::vector<std::string_view> prefixes;
  for (const auto& entry : entries.items())
    prefixes.push_back(model_name_prefix(entry.key()));
  std::ranges::sort(prefixes, str_ascii_icase_lt);
  prefixes.erase(std::ranges::unique(prefixes, str_ascii_icase_eq).begin(), prefixes.end());

  throw LinPipeError{"No model matches {}; the known model names are: {}", name, fmt::join(prefixes, ", ")};
}

bool ModelHub::matches_selector(std::string_view model_name, const Json& entry, const Selector& selector) {
  if (selector.key.empty() || selector.key == "name")
    return str_ascii_icase_eq(selector.value, model_name) || str_ascii_icase_eq(selector.value, model_name_prefix(model_name));
  if (selector.key == "language") return any_element_matches(entry, "languages", selector);
  if (selector.key == "operation") return any_element_matches(entry, "operations", selector);
  throw LinPipeError{"Unknown model selector key '{}'; the supported ones are 'name', 'language', and 'operation'", selector.key};
}

} // namespace linpipe
