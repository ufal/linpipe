// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/layer.h"
#include "layers/text.h"
#include "lib/json.h"

namespace linpipe {

unique_ptr<Layer> Layer::create(const string description) {
  if (description == "text") {
    return make_unique<layers::Text>(description);
  }

  throw LinpipeError{"Layer::create: Cannot construct invalid type of layer '", description, "'"};
}

const string& Layer::name() {
  return _name;
}

} // namespace linpipe
