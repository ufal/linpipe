// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/json/json.h"
#include "models/model_hub.h"
#include "utils/json_utils.h"

namespace linpipe {

namespace {

using ModelFactory = std::function<std::unique_ptr<Model>(const std::filesystem::path& dir, Json& config)>;
const std::unordered_map<std::string_view, ModelFactory>& model_factories() {
  static const std::unordered_map<std::string_view, ModelFactory> factories = {
    // {"udpipe", [](const auto& dir, Json& config) { return std::make_unique<UDPipeModel>(dir, config); }},
    // {"nametag", [](const auto& dir, Json& config) { return std::make_unique<NameTagModel>(dir, config); }},
  };
  return factories;
}

} // namespace

ModelHub::ModelHub(std::string repo_url) : asset_downloader_(repo_url.empty() ? std::string(default_repo_url_) : std::move(repo_url), std::string(family_)) {}

std::unique_ptr<Model> ModelHub::get_model(std::string_view name) {
  constexpr std::string_view caller = "ModelHub::get_model";

  // Throws on unknown model, missing model in offline mode, or download failure.
  auto model_dir = asset_downloader_.get_asset(name);

  Json config = json_parse_path(caller, model_dir / "model.json");
  auto family = json_extract_key<std::string>(caller, config, "family");

  const auto& factories = model_factories();
  auto factory = factories.find(family);
  if (factory == factories.end())
    throw LinpipeError{caller, ": Model ", name, " has an unknown family ", family};
  auto model = factory->second(model_dir, config);
  if (!model)
    throw LinpipeError{caller, ": Cannot load model ", name, " of family ", family};

  return model;
}

} // namespace linpipe
