// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/lif.h"
#include "lib/json.h"
#include "utils/json_utils.h"

namespace linpipe::formats {

std::unique_ptr<Document> Lif::load(std::istream& input, const std::string source_path) {
  std::string line;

  if (!getline(input, line))
    return nullptr;

  Json json = json_parse("Lif::load", line);

  auto document = std::make_unique<Document>();
  for (auto layer_json : json_get_array("Lif::load", json, "layers")) {
    json_assert_object("Lif::load", layer_json);

    std::string type = json_get_string("Lif::load", layer_json, "type");
    std::unique_ptr<Layer> layer = Layer::create(type);
    layer->from_json(layer_json);
    document->add_layer(std::move(layer));
  }

  document->set_source_path(source_path);

  return document;
}

void Lif::save(Document& document, std::ostream& output) {
  Json layers_json = Json::array();

  for (auto& layer : document.layers())
    layers_json.push_back(layer->to_json());

  Json json = {
    {"layers", layers_json},
  };

  output << json.dump() << std::endl;
}

} // namespace linpipe::formats
