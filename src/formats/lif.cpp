// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/lif.h"
#include "lib/json.h"
#include "utils/json_checker.h"

namespace linpipe::formats {

unique_ptr<Document> Lif::load(istream& input, const string source_path) {
  string line;

  if (!getline(input, line))
    return nullptr;

  Json json = Json::parse(line);

  JsonChecker json_checker;
  json_checker.json_has_array("Lif::load", json, "layers");

  auto document = make_unique<Document>();
  for (auto layer_json : json["layers"]) {
    json_checker.json_has_string("Lif::load", layer_json, "type");
    string type = layer_json["type"];

    string name;
    if (json.contains("name") && json.at("name").is_string()) {
      string name = json["name"];
    }

    unique_ptr<Layer> layer = Layer::create(name + ":" + type);
    layer->from_json(layer_json);
    document->add_layer(move(layer));
  }

  document->set_source_path(source_path);

  return document;
}

void Lif::save(Document& document, ostream& output) {
  Json layers_json = Json::array();

  for (auto& layer : document.layers()) {
    Json layer_json = Json();
    layer->to_json(layer_json);
    layers_json.push_back(layer_json);
  }

  Json json = Json::object();
  json["layers"] = layers_json;

  output << string(json.dump()) << endl;
}

} // namespace linpipe::formats
