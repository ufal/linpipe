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

  if (layer->_name.empty()) {
    layer->_name = "layer";
  }

  // If name already exists, add numbers until unique.
  if (unique_name_if_duplicate && _names.find(layer->_name) != _names.end()) {
    int i = 2;
    while (_names.find(layer->_name + "_" + to_string(i)) != _names.end()) i++;
    cerr << "Document::add_layer: Layer name '" << layer->_name << "' already exists in the document. Changed layer name to '" << layer->_name + "_" + to_string(i) << "'" << endl;
    layer->_name += "_" + to_string(i);
  }

  _names.insert(layer->_name);
  _layers.push_back(move(layer));

  return *_layers.back().get();
}

void Document::del_layer(const string_view name) {
  auto it = find_if(_layers.begin(), _layers.end(), [&](const unique_ptr<Layer>& l) { return l->name() == name; });

  if (it == _layers.end()) {
    throw LinpipeError{"Document::del_layer: Layer '", name, "' was not found in document."};
  }

  _layers.erase(it);
}

const vector<unique_ptr<Layer>>& Document::layers() {
  return _layers;
}

const string& Document::source_path() {
  return _source_path;
}

void Document::set_source_path(const string_view source_path) {
  _source_path = source_path;
}


} // namespace linpipe
