// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "models/model_manager.h"

namespace linpipe {

ModelManager ModelManager::singleton;

void ModelManager::reserve(const string name) {
  auto it = _reservations.find(name);
  if (it == _reservations.end()) {
    _reserved_models.push_back(name);
    _reservations.insert(pair<string, unsigned>(name, 1));
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
  auto it = _models.find(name);
  if (it != _models.end()) return it->second.get();

  // Load if capacity permits
  if (!_capacity || _models.size() < _capacity) {
    // TODO: Search for model in paths, open the istream, have the model loaded.
    return NULL;
  }
  else { // capacity exceeded
    // TODO: What to do?
    return NULL;
  }
}

void ModelManager::release(const string name) {
  auto it = _reservations.find(name);
  if (it == _reservations.end()) {
    // Not sure if we should throw an exception, perhaps we should just silently continue?
    throw LinpipeError{"ModelManager::release: Model '", name, "' cannot be released because it has not been reserved."};
  }

  // Decrease number of model reservations.
  it->second -= 1;

  // If no model reservations left, remove from reservations and unload.
  if (it->second == 0) {
    _reservations.erase(it);
  }

  //auto it = _reserved_models.find(name);
  //if (it != _reserved_models.end()) _reserved_models.erase(it);

  //auto it = _models.find(name);
  //if (it != _models.end()) _models.erase(it);
}

} // namespace linpipe
