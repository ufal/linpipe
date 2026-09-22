// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "core/document.h"
#include "layers/layer.h"
#include "layers/plain_text.h"
#include "layers/token_layer.h"
#include "lib/doctest.h"

namespace linpipe {

TEST_CASE("Document::get_layer") {
  Document doc;

  SUBCASE("throws exception when document has no layers and no name given") {
    CHECK_THROWS_AS(doc.get_layer(), LinpipeError);
  }

  SUBCASE("throws exception when document has no layers and name given") {
    CHECK_THROWS_AS(doc.get_layer("plain_text"), LinpipeError);
  }

  unique_ptr<Layer> layer = Layer::create("plain_text");
  doc.add_layer(std::move(layer));
  unique_ptr<Layer> layer2 = Layer::create("plain_text");
  doc.add_layer(std::move(layer2));

  SUBCASE("returns last layer if no type or name given") {
    CHECK_NOTHROW(doc.get_layer());
    CHECK(doc.get_layer().name() == "plain_text_2");
  }

  SUBCASE("returns layer of given name") {
    CHECK_NOTHROW(doc.get_layer("plain_text"));
    CHECK_NOTHROW(doc.get_layer<layers::PlainText>("plain_text"));
    CHECK(doc.get_layer("plain_text").name() == "plain_text");
  }

  SUBCASE("throws exception when layer of given name not found") {
    CHECK_THROWS_AS(doc.get_layer("bad_name"), LinpipeError);
  }
}

TEST_CASE("Document::get_layer<T>") {
  Document doc;

  SUBCASE("throws exception when document has no layers and no name given") {
    CHECK_THROWS_AS(doc.get_layer<layers::PlainText>(), LinpipeError);
  }

  SUBCASE("throws exception when document has no layers and name given") {
    CHECK_THROWS_AS(doc.get_layer<layers::PlainText>("plain_text"), LinpipeError);
  }

  unique_ptr<Layer> layer = Layer::create("plain_text");
  doc.add_layer(std::move(layer));
  unique_ptr<Layer> layer2 = Layer::create("plain_text");
  doc.add_layer(std::move(layer2));

  SUBCASE("returns last layer of the requested type if no name given") {
    CHECK_NOTHROW(doc.get_layer<layers::PlainText>());
    CHECK(doc.get_layer<layers::PlainText>().name() == "plain_text_2");
  }

  SUBCASE("returns layer of given name") {
    CHECK_NOTHROW(doc.get_layer<layers::PlainText>("plain_text"));
    CHECK(doc.get_layer<layers::PlainText>("plain_text").name() == "plain_text");
  }

  SUBCASE("throws exception when layer of given name not found") {
    CHECK_THROWS_AS(doc.get_layer<layers::PlainText>("bad_name"), LinpipeError);
  }
}

TEST_CASE("Document::add_layer") {
  Document doc;

  SUBCASE("adding layer increases layers vector size of the document") {
    doc.add_layer(Layer::create("plain_text"));
    CHECK(doc.layers().size() == 1);
  }

  SUBCASE("layer without name gets default name") {
    doc.add_layer(Layer::create("plain_text"));
    CHECK(doc.layers()[0]->name() == "plain_text");
  }

  SUBCASE("layer with conflicting name gets unique name") {
    doc.add_layer(Layer::create("plain_text"));
    doc.add_layer(Layer::create("plain_text"));
    CHECK(doc.layers()[0]->name() == "plain_text");
    CHECK(doc.layers()[1]->name() == "plain_text_2");
  }
}

TEST_CASE("Document::del_layer") {
  Document doc;

  SUBCASE("throws exception when attempting to erase from empty layers") {
    CHECK_THROWS_AS(doc.del_layer("plain_text"), LinpipeError);
  }

  unique_ptr<Layer> layer = Layer::create("plain_text");
  doc.add_layer(std::move(layer));

  SUBCASE("throws exception when attempting to erase non-existent layer") {
    CHECK_THROWS_AS(doc.del_layer("bad_name"), LinpipeError);
  }

  SUBCASE("deleting layer decreases size of layers") {
    doc.del_layer("plain_text");
    CHECK(doc.layers().size() == 0);
  }
}

} // namespace linpipe
