// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/layer.h"
#include "layers/lemmas.h"
#include "layers/spans.h"
#include "layers/plain_text.h"
#include "layers/token_layer.h"
#include "lib/json.h"

namespace linpipe {

unique_ptr<Layer> Layer::create(const string type, const string name) {
  // Construct layer of corresponding type.
  if (type == "spans")
    return make_unique<layers::Spans>(name);
  if (type == "lemmas")
    return make_unique<layers::Lemmas>(name);
  if (type == "plain_text")
    return make_unique<layers::PlainText>(name);
  if (type == "token_layer")
    return make_unique<layers::TokenLayer>(name);

  throw LinpipeError{"Layer::create: Cannot construct layer of unknown type '", type, "'"};
}

} // namespace linpipe
