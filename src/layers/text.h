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
#include "layers/layer.h"

namespace linpipe::layers {

class Text : public Layer {
 public:
  Text(const string name={}) : Layer("text", name.empty() ? "text" : name) {};

  virtual void from_json(const Json& json) override;
  virtual Json to_json() override;
  virtual string to_html() override;

  string text;
};

} // namespace linpipe::layers
