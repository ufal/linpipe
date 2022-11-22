// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/layer.h"
#include "lib/doctest.h"

namespace linpipe {

TEST_CASE("Layer::create") {

  SUBCASE("creates layer of name and type") {
    unique_ptr<Layer> layer = Layer::create("name:text");
    CHECK(layer->name() == "name");
    CHECK(layer->type() == "text");
  }

  SUBCASE("creates layer of type text and default name") {
    unique_ptr<Layer> layer = Layer::create("text");
    CHECK(layer->name() == "text");
    CHECK(layer->type() == "text");

    unique_ptr<Layer> layer2 = Layer::create(":text");
    CHECK(layer2->name() == "text");
    CHECK(layer2->type() == "text");
  }

  SUBCASE("creates layer of type tokens and default name") {
    unique_ptr<Layer> layer = Layer::create("tokens");
    CHECK(layer->name() == "tokens");
    CHECK(layer->type() == "tokens");

    unique_ptr<Layer> layer2 = Layer::create(":tokens");
    CHECK(layer2->name() == "tokens");
    CHECK(layer2->type() == "tokens");
  }

}

} // namespace linpipe
