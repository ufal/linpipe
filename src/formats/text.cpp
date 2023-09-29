// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/text.h"
#include "layers/text.h"
#include "lib/json.h"

namespace linpipe::formats {

unique_ptr<Document> Text::load(istream& input, const string source_path) {
  if (input.eof())
    return nullptr;

  unique_ptr<layers::Text> layer = make_unique<layers::Text>();

  char block[4096];
  while (input.read(block, sizeof(block)))
    layer->text.append(block, sizeof(block));
  layer->text.append(block, input.gcount());

  auto document = make_unique<Document>();
  document->add_layer(std::move(layer));
  document->set_source_path(source_path);

  return document;
}

void Text::save(Document& document, ostream& output) {
  auto& layer = document.get_layer<layers::Text>();

  output << string(layer.text);
}

} // namespace linpipe::formats
