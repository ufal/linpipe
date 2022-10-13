// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"
#include "core/model.h"

namespace linpipe {

class ModelManager {
 public:
  void reserve(const string_view name);
  Model* load(const string_view name);
  void release(const string_view name);

  // It is possible to configure when the models are unloaded after release,
  // with default being never to unload.

  static ModelManager singleton;

 private:
  unordered_map<string, unique_ptr<Model>> _models;
};

} // namespace linpipe
