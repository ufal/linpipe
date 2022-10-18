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

TEST_CASE("Document::add_layer") {
  Document doc;

  SUBCASE("adding layer increases layers vector size of the document") {
    doc.add_layer(make_unique<layers::Text>());
    CHECK(doc.layers().size() == 1);
  }
}

} // namespace linpipe
