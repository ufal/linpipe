// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <functional>

#include "core/layer.h"
#include "core/view.h"
#include "layers/plain_text_layer.h"
#include "utils/json_utils.h"

namespace linpipe {

std::unique_ptr<Layer> Layer::create(std::string_view type, std::string_view name) {
  if (type == PlainTextLayer::TYPE) return std::make_unique<PlainTextLayer>(name);

  throw LinPipeError("Cannot create a layer of an unknown type '{}'", type);
}

std::unique_ptr<Layer> Layer::create_from_json(Json& json) {
  auto type = json_extract<std::string>(json, "type");
  auto name = json_extract<std::string>(json, "name");
  auto layer = create(type, name);
  layer->from_json(json);
  return layer;
}

void Layer::from_json(Json& json) {
  if (json.contains("model")) model = json_extract<std::string>(json, "model");
  if (json.contains("license")) license = json_extract<std::string>(json, "license");
  if (json.contains("timestamp")) timestamp = json_extract<std::string>(json, "timestamp");
}

Json Layer::to_json() const {
  Json json{{"type", type()}, {"name", name}};
  if (!model.empty()) json["model"] = model;
  if (!license.empty()) json["license"] = license;
  if (!timestamp.empty()) json["timestamp"] = timestamp;
  return json;
}

bool Layer::matches(std::span<const Selector> selectors) const {
  return std::ranges::all_of(selectors, std::bind_front(&Layer::matches_selector, this));
}

bool Layer::matches_selector(const Selector& selector) const {
  if (selector.key.empty() || selector.key == "type") return selector.matches(type());
  if (selector.key == "name") return selector.matches(name);
  if (selector.key == "model") return selector.matches(model);
  return false;
}

std::unique_ptr<View> Layer::create_view(std::span<const Selector> selectors) const {
  auto provides = [selectors](const ViewProvider& provider) {
    return std::ranges::all_of(selectors, [&provider](const Selector& selector) {
      return std::ranges::any_of(provider.selectors, [&selector](const Selector& offered) {
        return (selector.key == offered.key || (selector.key.empty() && offered.key == "type")) && selector.matches(offered.value);
      });
    });
  };

  auto providers = views();
  auto provider = std::ranges::find_if(providers, provides);
  if (provider == providers.end())
    throw LinPipeError("No view of layer '{}:type={}' matches the selectors '{}'", name, type(), selectors);
  if (std::ranges::any_of(std::next(provider), providers.end(), provides))
    throw LinPipeError("Multiple views of layer '{}:type={}' match the selectors '{}'", name, type(), selectors);

  return provider->create(*this);
}

} // namespace linpipe
