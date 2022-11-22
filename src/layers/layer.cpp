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
#include "layers/tokens.h"
#include "lib/json.h"
#include "utils/string_helper.h"

namespace linpipe {

unique_ptr<Layer> Layer::create(const string description) {
  // Get layer type.
  string type;
  string name;
  StringHelper string_helper;
  if (description.find(":") != string::npos) {
    vector<string> tokens;
    string_helper.split(tokens, description, ":");
    if (tokens.size() != 2) {
        throw LinpipeError{"Expected name:format in layer description '", description, "'"};
      }
    name = tokens[0];
    type = tokens[1];
  }
  else {
    type = description;
  }

  // Construct layer of corresponding type.
  if (type == "text") {
    return make_unique<layers::Text>(name);
  }
  if (type == "tokens") {
    return make_unique<layers::Tokens>(name);
  }

  throw LinpipeError{"Layer::create: Cannot construct unknown type of layer '", description, "'"};
}

} // namespace linpipe
