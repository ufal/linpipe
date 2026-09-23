// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/layer.h"
#include "lib/doctest.h"

namespace linpipe {

TEST_CASE("Layer::create") {

  SUBCASE("creates layer of name and type") {
    std::unique_ptr<Layer> layer = Layer::create("plain_text", "name");
    CHECK(layer->name() == "name");
    CHECK(layer->type() == "plain_text");
  }

  SUBCASE("creates layer of type text and default name") {
    std::unique_ptr<Layer> layer = Layer::create("plain_text");
    CHECK(layer->name() == "plain_text");
    CHECK(layer->type() == "plain_text");
  }

  SUBCASE("creates layer of type token layer and default name") {
    std::unique_ptr<Layer> layer = Layer::create("token_layer");
    CHECK(layer->name() == "token_layer");
    CHECK(layer->type() == "token_layer");
  }

}

} // namespace linpipe
