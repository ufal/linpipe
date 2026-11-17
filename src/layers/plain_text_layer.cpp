// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/plain_text_layer.h"
#include "utils/json_utils.h"
#include "views/plain_text_view.h"

namespace linpipe {

namespace {

class PlainTextLayerView : public PlainTextView {
 public:
  explicit PlainTextLayerView(const PlainTextLayer& layer) : PlainTextView(layer), layer_(&layer) {}

  std::string_view text() const override { return layer_->text; }

 private:
  const PlainTextLayer* layer_;
};

} // namespace

void PlainTextLayer::from_json(Json& json) {
  Layer::from_json(json);
  text = json_extract<std::string>(json, "text");
}

Json PlainTextLayer::to_json() const {
  Json json = Layer::to_json();
  json["text"] = text;
  return json;
}

std::string PlainTextLayer::to_html() const {
  return {};
}

std::span<const ViewProvider> PlainTextLayer::provided_views() {
  static const std::vector<ViewProvider> providers{
    {{{"type", std::string(PlainTextView::TYPE)}},
     [](const Layer& layer) -> std::unique_ptr<View> {
       return std::make_unique<PlainTextLayerView>(static_cast<const PlainTextLayer&>(layer));
     }},
  };
  return providers;
}

} // namespace linpipe
