// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "core/document.h"
#include "layers/text.h"
#include "lib/doctest.h"

namespace linpipe {

TEST_CASE("Document::get_layer") {
  Document doc;

  SUBCASE("throws exception when document has no layers") {
    CHECK_THROWS_AS(doc.get_layer("test"), LinpipeError);
  }

  unique_ptr<layers::Text> layer = make_unique<layers::Text>();
  layer->set_name("test");
  doc.add_layer(move(layer));

  SUBCASE("gets layer of given name") {
    CHECK_NOTHROW(doc.get_layer("test"));
    CHECK(doc.get_layer("test").name() == "test");
  }

  SUBCASE("throws exception when layer of given name not found") {
    CHECK_THROWS_AS(doc.get_layer("bad_name"), LinpipeError);
  }

}

TEST_CASE("Document::add_layer") {
  Document doc;

  SUBCASE("adding layer increases layers vector size of the document") {
    doc.add_layer(make_unique<layers::Text>());
    CHECK(doc.layers().size() == 1);
  }
}

TEST_CASE("Document::del_layer") {
  Document doc;

  SUBCASE("throws exception when attempting to erase from empty layers") {
    CHECK_THROWS_AS(doc.del_layer("test"), LinpipeError);
  }

  unique_ptr<layers::Text> layer = make_unique<layers::Text>();
  layer->set_name("test");
  doc.add_layer(move(layer));

  SUBCASE("throws exception when attempting to erase non-existent layer") {
    CHECK_THROWS_AS(doc.del_layer("bad_name"), LinpipeError);
  }

  SUBCASE("deleting layer decreases size of layers") {
    doc.del_layer("test");
    CHECK(doc.layers().size() == 0);
  }

}

} // namespace linpipe
