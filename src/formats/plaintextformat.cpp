// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/plaintextformat.h"
#include "layers/plaintext.h"

namespace linpipe {

bool PlainTextFormat::load(Document& document, istream& input, string_view source_path) {
  stringstream strStream;
  strStream << input.rdbuf();

  unique_ptr<PlainText> layer = unique_ptr<PlainText>(new PlainText());
  layer->text = strStream.str();

  document.add_layer(move(layer), "text");

  return !input.eof();
}

void PlainTextFormat::save(const Document& document, ostream& output) {
}

} // namespace linpipe
