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

namespace linpipe {

class Document;

class Layer {
 public:
  virtual ~Layer() {}

  virtual void from_json(const Json& json) = 0;
  virtual void to_json(Json& json) = 0;
  virtual void to_html(string& html) = 0;

  const string& name();
  void set_name(const string_view name);

  // TODO: add more structured metadata info

 protected:
  string _name;
  friend class Document;

  // TODO: move this to layers/utils
  void _json_has_string(const string_view called_from, const Json& json, const string_view key);
};

} // namespace linpipe
