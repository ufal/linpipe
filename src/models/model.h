// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <map>

#include "common.h"
#include "lib/json/json.h"

namespace linpipe {

using ModelRequirements = std::map<std::string, std::filesystem::path, std::less<>>;

class Model {
 public:
  const std::filesystem::path& dir() const { return dir_; }
  const Json& config() const { return config_; }
  const ModelRequirements& requirements() const { return requirements_; }
  const std::filesystem::path& requirement(std::string_view name) const {
    auto it = requirements_.find(name);
    if (it == requirements_.end())
      throw LinPipeError{"Model requirement {} is not available", name};
    return it->second;
  }
  virtual ~Model() = 0;

 protected:
  explicit Model(std::filesystem::path dir = {}, Json config = {}, ModelRequirements requirements = {})
      : dir_(std::move(dir)), config_(std::move(config)), requirements_(std::move(requirements)) {}

 private:
  std::filesystem::path dir_;
  Json config_;
  ModelRequirements requirements_;
};

inline Model::~Model() = default;

} // namespace linpipe
