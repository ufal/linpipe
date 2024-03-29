// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "models/model_manager.h"

namespace linpipe {

ModelManager ModelManager::singleton;

void ModelManager::reserve(const string name) {
  auto it = reservations_.find(name);
  if (it == reservations_.end()) {
    reserved_models_.push_back(name);
    reservations_.insert(pair<string, unsigned>(name, 1));
  }
  else {
    it->second += 1;
  }

  if (load_on_reserve) {
    this->load(name);
  }
}

Model* ModelManager::load(const string name) {
  // Return model if model already loaded
  auto it = models_.find(name);
  if (it != models_.end()) return it->second.get();

  // Load if capacity permits
  if (!capacity_ || models_.size() < capacity_) {
    // TODO: Search for model in paths, open the istream, have the model loaded.
    return NULL;
  }
  else { // capacity exceeded
    // TODO: What to do?
    return NULL;
  }
}

void ModelManager::release(const string name) {
  auto it = reservations_.find(name);
  if (it == reservations_.end()) {
    // Not sure if we should throw an exception, perhaps we should just silently continue?
    throw LinpipeError{"ModelManager::release: Model '", name, "' cannot be released because it has not been reserved."};
  }

  // Decrease number of model reservations.
  it->second -= 1;

  // If no model reservations left, remove from reservations and unload.
  if (it->second == 0) {
    reservations_.erase(it);
  }

  auto it2 = find(reserved_models_.begin(), reserved_models_.end(), name);
  if (it2 != reserved_models_.end()) reserved_models_.erase(it2);

  auto it3 = models_.find(name);
  if (it3 != models_.end()) models_.erase(it3);
}

} // namespace linpipe
