// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"
#include "models/model.h"

namespace linpipe {

class ModelManager {
 public:
  void reserve(const std::string name);
  Model* load(const std::string name);
  void release(const std::string name);

  // It is possible to configure when the models are unloaded after release,
  // with default being never to unload.

  static ModelManager singleton;

 private:
  // Model reservations
  std::vector<std::string> reserved_models_;  // model names FIFO
  std::unordered_map<std::string, unsigned> reservations_;  // model reservations counts

  // Models currently held in memory
  std::unordered_map<std::string, std::unique_ptr<Model>> models_;

  // Maximum capacity of models held in memory. 0 means infinity.
  unsigned capacity_ = 0;

  // Load on reserve? If true, load immediately.
  bool load_on_reserve = true;
};

} // namespace linpipe
