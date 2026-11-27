// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/plain_text_layer.h"
#include "lib/doctest/doctest.h"
#include "lib/json/json.h"
#include "views/plain_text_view.h"

namespace linpipe {

TEST_CASE("PlainTextLayer") {
  PlainTextLayer plain_text;
  CHECK(plain_text.type() == "plain_text");
  CHECK(plain_text.name == "plain_text");
  CHECK(PlainTextLayer("text").name == "text");
}

TEST_CASE("PlainTextLayer Layer::create") {
  auto layer = Layer::create("plain_text", "text");
  CHECK(layer->type() == "plain_text");
  CHECK(layer->name == "text");
  CHECK(dynamic_cast<PlainTextLayer*>(layer.get()));

  CHECK_THROWS_AS(Layer::create("unknown"), LinPipeError);
}

TEST_CASE("PlainTextLayer Layer::move_to_unique") {
  PlainTextLayer plain_text("text");
  plain_text.text = "hello world";
  plain_text.model = "udpipe";

  Layer& layer = plain_text;
  auto moved = layer.move_to_unique();
  CHECK(moved->type() == "plain_text");
  CHECK(moved->name == "text");
  CHECK(moved->model == "udpipe");
  CHECK(dynamic_cast<PlainTextLayer&>(*moved).text == "hello world");

  CHECK(plain_text.name == "plain_text");
  CHECK(plain_text.model.empty());
  CHECK(plain_text.text.empty());
}

TEST_CASE("PlainTextLayer Layer::matches") {
  PlainTextLayer layer("text");
  layer.model = "udpipe";

  CHECK(layer.matches({}));
  CHECK(layer.matches(std::vector<Selector>{{"type", "plain_text"}}));
  CHECK(layer.matches(std::vector<Selector>{{"", "plain_text"}}));
  CHECK_FALSE(layer.matches(std::vector<Selector>{{"", "text"}}));
  CHECK(layer.matches(std::vector<Selector>{{"name", "text"}, {"model", "udpipe"}}));
  CHECK_FALSE(layer.matches(std::vector<Selector>{{"name", "other"}}));
  CHECK_FALSE(layer.matches(std::vector<Selector>{{"name", "text"}, {"model", "other"}}));
  CHECK_FALSE(layer.matches(std::vector<Selector>{{"unknown", "text"}}));
}

class OtherView : public TypedView<OtherView> {
 public:
  static constexpr std::string_view TYPE = "other";
};

TEST_CASE("PlainTextLayer Layer::create_view") {
  PlainTextLayer layer("text");
  layer.text = "hello world";
  layer.model = "udpipe";
  layer.license = "CC BY-SA";
  layer.timestamp = "2026-01-01T12:00:00Z";

  REQUIRE(layer.views().size() == 1);
  REQUIRE(layer.views()[0].selectors.size() == 1);
  CHECK(layer.views()[0].selectors[0].key == "type");
  CHECK(layer.views()[0].selectors[0].value == "plain_text");

  for (auto selectors : {std::vector<Selector>{}, std::vector<Selector>{{"type", "plain_text"}}, std::vector<Selector>{{"", "plain_text"}}}) {
    auto view = layer.create_view(selectors);
    CHECK(view->type() == "plain_text");
    CHECK(dynamic_cast<PlainTextView&>(*view).text() == "hello world");
    CHECK(view->model() == "udpipe");
    CHECK(view->license() == "CC BY-SA");
    CHECK(view->timestamp() == "2026-01-01T12:00:00Z");
  }

  CHECK_THROWS_AS(layer.create_view(std::vector<Selector>{{"", "other"}}), LinPipeError);
  CHECK_THROWS_AS(layer.create_view(std::vector<Selector>{{"type", "plain_text"}, {"name", "text"}}), LinPipeError);

  std::unique_ptr<PlainTextView> view = layer.create_view<PlainTextView>({});
  CHECK(view->text() == "hello world");
  CHECK(layer.create_view<View>({})->type() == "plain_text");
  CHECK_THROWS_AS(layer.create_view<OtherView>({}), LinPipeError);

  CHECK(layer.create_view(std::vector<Selector>{{"", "plain_text"}}, false));
  CHECK_FALSE(layer.create_view(std::vector<Selector>{{"", "other"}}, false));
  CHECK(layer.create_view<PlainTextView>({}, false));
  CHECK_FALSE(layer.create_view<PlainTextView>(std::vector<Selector>{{"", "other"}}, false));
  CHECK_FALSE(layer.create_view<OtherView>({}, false));
}

TEST_CASE("PlainTextLayer Layer::create_from_json") {
  auto create_from_json = [](Json json) { return Layer::create_from_json(json); };
  CHECK_THROWS_AS(create_from_json(Json(42)), LinPipeError);
  CHECK_THROWS_AS(create_from_json(Json{{"name", "text"}, {"text", "hello world"}}), LinPipeError);
  CHECK_THROWS_AS(create_from_json(Json{{"type", "plain_text"}, {"text", "hello world"}}), LinPipeError);
  CHECK_THROWS_AS(create_from_json(Json{{"type", "unknown"}, {"name", "text"}, {"text", "hello world"}}), LinPipeError);
  CHECK_THROWS_AS(create_from_json(Json{{"type", "plain_text"}, {"name", "text"}}), LinPipeError);
  CHECK_THROWS_AS(create_from_json(Json{{"type", "plain_text"}, {"name", "text"}, {"text", 42}}), LinPipeError);

  Json json{{"type", "plain_text"}, {"name", "text"}, {"text", "hello world"}};
  auto layer = create_from_json(json);
  CHECK(layer->type() == "plain_text");
  CHECK(layer->name == "text");
  CHECK(dynamic_cast<PlainTextLayer&>(*layer).text == "hello world");
  CHECK(layer->model.empty());
  CHECK(layer->license.empty());
  CHECK(layer->timestamp.empty());
  CHECK(layer->to_json() == json);

  json.update(Json{{"model", "LinPipe"}, {"license", "CC BY-SA"}, {"timestamp", "2026-01-01T12:00:00Z"}});
  layer = create_from_json(json);
  CHECK(layer->model == "LinPipe");
  CHECK(layer->license == "CC BY-SA");
  CHECK(layer->timestamp == "2026-01-01T12:00:00Z");
  CHECK(layer->to_json() == json);

  CHECK_THROWS_AS(create_from_json(Json{{"type", "plain_text"}, {"name", "text"}, {"text", "hello world"}, {"model", 42}}), LinPipeError);
}

} // namespace linpipe
