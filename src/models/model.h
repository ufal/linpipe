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
#include "lib/json/json.h"

namespace linpipe {

class Model {
 public:
  const std::filesystem::path& dir() const { return dir_; }
  const Json& config() const { return config_; }
  virtual ~Model() = 0;

 protected:
  explicit Model(std::filesystem::path dir = {}, Json config = {})
      : dir_(std::move(dir)), config_(std::move(config)) {}

 private:
  std::filesystem::path dir_;
  Json config_;
};

inline Model::~Model() = default;

} // namespace linpipe
