// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/text.h"
#include "layers/text.h"
#include "lib/json.h"

namespace linpipe::formats {

bool Text::load(Document& document, istream& input, const string_view source_path) {
  stringstream strStream;
  strStream << input.rdbuf();

  unique_ptr<layers::Text> layer = make_unique<layers::Text>();
  layer->text = strStream.str();
  layer->set_name(_name);

  document.add_layer(move(layer));
  document.set_source_path(source_path);

  return !input.eof();
}

void Text::save(Document& document, ostream& output) {
  Layer& layer = document.get_layer(_name);

  Json json = Json::object();
  layer.to_json(json);

  output << json["text"];
}

} // namespace linpipe::formats
