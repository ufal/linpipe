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

class TokenizedText : public Layer {
 public:
  virtual void from_json(const Json& json) override;
  virtual void to_json(Json& json) override;
  virtual void to_html(string& html) override;

  vector<string> tokens;
};

} // namespace linpipe::layers
