// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <set>

#include "common.h"
#include "layers/layer.h"

namespace linpipe {

class Document {
 public:
  // The responsibility of the Document is to guarantee that layers
  // have unique name.
  template<typename T=Layer> T& get_layer(const string_view name={});
  Layer& add_layer(unique_ptr<Layer>&& layer, bool unique_name_if_duplicate=true);
  void del_layer(const string_view name);

  const vector<unique_ptr<Layer>>& layers();
  const string& source_path();

  void set_source_path(const string_view source_path);

 private:
  vector<unique_ptr<Layer>> _layers;
  set<string> _names;

  string _source_path;
};

// Definitions

template<> inline Layer& Document::get_layer(const string_view name) {
  /* Gets layer by name or the last layer if no name given.

  Receives:
    name: string_view with name of the layer, empty by default.

  Returns:
    reference to a layer of the given name. If name is empty, returns the last
    layer.

  Throws:
    LinpipeError if document has no layers.
    LinpipeError if layer of given name is not found in the document.
  */

  if (name.empty()) {
    if (_layers.empty()) {
      throw LinpipeError{"Document::get_layer: Last layer of document requested but document has no layers."};
    }
    return *_layers[_layers.size()-1].get();
  }

  for (auto& it : _layers)
    if (it->name() == name)
      return *it;

  throw LinpipeError{"Document::get_layer: Layer '", name, "' was not found in document."};
}

template<typename T> T& Document::get_layer(const string_view name) {
  /* Gets layer by name and type or the last layer of the type if no name given.

  Receives:
    name: string_view with name of the layer, empty by default.

  Returns:
    reference to a layer of the given name. If name is empty, returns the last
    layer of the type.

  Throws:
    LinpipeError if document has no layers.
    LinpipeError if layer of given name is not found in the document.
    LinpipeError if layer of given name is not of requested type.
    LinpipeError if name is empty but no layer of requested type is found.
  */

  if (name.empty()) {
    for (auto it = _layers.rbegin(); it != _layers.rend(); ++it) {
      T* layer = dynamic_cast<T*>(it->get());
      if (layer) {  // layer of type T found
        return *layer;
      }
    }
    throw LinpipeError{"Document::get_layer: Layer of requested type '", T().type(), "' was not found in document."};
  }

  for (auto& it : _layers)
    if (it->name() == name) {
      T* layer = dynamic_cast<T*>(it.get());
      if (!layer)
        throw LinpipeError{"Document::get_layer: Layer '", name, "' was not of expected type '", T().type(), "'"};
      return *layer;
    }

  throw LinpipeError{"Document::get_layer: Layer '", name, "' was not found in document."};
}


} // namespace linpipe
