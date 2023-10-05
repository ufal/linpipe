// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "core/document.h"

namespace linpipe {

Layer& Document::add_layer(unique_ptr<Layer>&& layer, bool unique_name_if_duplicate) {
  /* Adds layer to collection of layers, maintaining unique layer names by default.

  Receives:
    layer: unique ptr to layer
    unique_name_if_duplicate: bool, if true and name already exists in
      collection, create a new name. If false, keep duplicate name. Default:
      true.
      TODO: Do we want to allow duplicate names at all? Shouldn't the unique
      naming be required? Then this argument would go away.

  Returns:
    reference to the recently added layer in collection
  */

  if (layer->name_.empty()) {
    layer->name_ = "layer";
  }

  // If name already exists, add numbers until unique.
  if (unique_name_if_duplicate && names_.find(layer->name_) != names_.end()) {
    int i = 2;
    while (names_.find(layer->name_ + "_" + to_string(i)) != names_.end()) i++;
    layer->name_ += "_" + to_string(i);
  }

  names_.insert(layer->name_);
  layers_.push_back(std::move(layer));

  return *layers_.back().get();
}

void Document::del_layer(const string_view name) {
  auto it = find_if(layers_.begin(), layers_.end(), [&](const unique_ptr<Layer>& l) { return l->name() == name; });

  if (it == layers_.end()) {
    throw LinpipeError{"Document::del_layer: Layer '", name, "' was not found in document."};
  }

  layers_.erase(it);
}

const vector<unique_ptr<Layer>>& Document::layers() {
  return layers_;
}

const string& Document::source_path() {
  return source_path_;
}

void Document::set_source_path(const string_view source_path) {
  source_path_ = source_path;
}


} // namespace linpipe
