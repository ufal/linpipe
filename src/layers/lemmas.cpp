// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "layers/lemmas.h"
#include "lib/json.h"

namespace linpipe::layers {

void Lemmas::from_json(const Json& /*json*/) {
}

Json Lemmas::to_json() {
  return {
    {"type", _type},
    {"name", _name},
    {"lemmas", lemmas},
    {"token_layer", token_layer},
  };
}

string Lemmas::to_html() {
  return string();
}

} // namespace linpipe::layers
