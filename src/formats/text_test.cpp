// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <sstream>

#include "core/document.h"
#include "formats/text.h"
#include "layers/text.h"
#include "lib/doctest.h"
#include "lib/json.h"

namespace linpipe {

TEST_CASE("formats::Text::load") {
  unique_ptr<Format> text = Format::create("text");
  Document doc;

  SUBCASE("loads text, adds text layer with default name") {
    istringstream is("Hello world!");
    text->load(doc, is, "source");

    CHECK(doc.layers().size() == 1);
    CHECK(doc.source_path() == "source");
    CHECK(doc.layers()[0]->name() == "text");

    linpipe::layers::Text* layer = dynamic_cast<linpipe::layers::Text*>(&doc.get_layer("text"));
    CHECK(layer->text == "Hello world!");
  }

  SUBCASE("loads text, add text layer with given name") {
    istringstream is("Hello world!");
    text->set_name("custom_layer_name");
    text->load(doc, is, "source");
    CHECK(doc.layers()[0]->name() == "custom_layer_name");
  }
}

} // namespace linpipe::formats
