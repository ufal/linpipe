// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <functional>
#include <unordered_map>

#include "models/model_factory.h"
#include "utils/json_utils.h"

// Individual models includes
// #include "models/udpipe.h"
// ...

namespace linpipe {

namespace {

using Factory = std::function<std::unique_ptr<Model>(const std::filesystem::path& dir, Json& config)>;

template<typename M>
std::unique_ptr<Model> make(const std::filesystem::path& dir, Json& config) {
  return std::make_unique<M>(dir, config);
}

const std::unordered_map<std::string_view, Factory>& factories() {
  static const std::unordered_map<std::string_view, Factory> factories = {
    // {"UDPipe", make<UDPipe>},
  };
  return factories;
}

} // namespace

std::unique_ptr<Model> create_model(std::string_view name, const std::filesystem::path& dir, Json& config) {
  auto family = json_extract<std::string>(config, "family");
  auto factory = factories().find(family);
  if (factory == factories().end())
    throw LinpipeError{"Model {} has an unknown family {}", name, family};

  auto model = factory->second(dir, config);
  if (!model)
    throw LinpipeError{"Cannot load model {} of family {}", name, family};
  return model;
}

} // namespace linpipe
