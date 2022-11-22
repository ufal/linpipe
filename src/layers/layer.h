// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "utils/json_checker.h"

namespace linpipe {

class Document;

class Layer {
 public:
  virtual ~Layer() {}

  static unique_ptr<Layer> create(const string type, const string name={});

  virtual void from_json(const Json& json) = 0;
  virtual Json to_json() = 0;
  virtual string to_html() = 0;

  const string& name() { return _name; }
  const string& type() { return _type; }

  // TODO: add more structured metadata info

 protected:
  Layer(const string type, const string name) : _type(type), _name(name) {};

  string _type;
  string _name;
  friend class Document;

  JsonChecker _json_checker;
};

} // namespace linpipe
