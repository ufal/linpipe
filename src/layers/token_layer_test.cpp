// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/plain_text.h"
#include "layers/token_layer.h"
#include "lib/doctest.h"
#include "lib/json.h"

namespace linpipe {

namespace {

void check_token(const layers::Token& token, const std::string& text, int begin, int end) {
  CHECK(token.text == text);
  CHECK(token.index_span.begin == begin);
  CHECK(token.index_span.end == end);
}

Json layer_json(const Json& tokens) {
  return Json{{"type", "token_layer"}, {"name", "tokens"}, {"tokens", tokens}};
}

std::vector<layers::IndexSpan> spans(const SentenceView& view) {
  std::vector<layers::IndexSpan> result;
  for (size_t i = 0; i < view.size(); i++)
    result.push_back(view.span(i));
  return result;
}

std::vector<std::string> texts(const TokenView& view) {
  std::vector<std::string> result;
  for (size_t i = 0; i < view.size(); i++)
    result.emplace_back(view.text(i));
  return result;
}

std::vector<layers::IndexSpan> normalize(size_t ntokens, const std::vector<layers::IndexSpan>& sentences) {
  return spans(layers::TokenLayerSentenceView(sentences, ntokens));
}

} // namespace

TEST_CASE("TokenLayer::TokenLayer") {
  SUBCASE("uses default name") {
    layers::TokenLayer layer;
    CHECK(layer.name() == "token_layer");
    CHECK(layer.type() == "token_layer");
    CHECK(layer.plain_text == nullptr);
  }

  SUBCASE("stores name and plain text") {
    layers::PlainText plain_text;
    layers::TokenLayer layer("tokens", &plain_text);
    CHECK(layer.name() == "tokens");
    CHECK(layer.plain_text == &plain_text);
  }
}

TEST_CASE("TokenLayer::token_view") {
  layers::PlainText plain_text;
  plain_text.text = "Hello world!";

  SUBCASE("reports size") {
    layers::TokenLayer layer;
    CHECK(layer.token_view()->size() == 0);
    layer.tokens.emplace_back("a");
    layer.tokens.emplace_back("b");
    CHECK(layer.token_view()->size() == 2);
  }

  SUBCASE("returns token text") {
    layers::TokenLayer layer("tokens", &plain_text);
    layer.tokens.emplace_back("Hello");
    CHECK(layer.token_view()->text(0) == "Hello");
  }

  SUBCASE("text takes precedence over span") {
    layers::TokenLayer layer("tokens", &plain_text);
    layer.tokens.emplace_back("Howdy", layers::IndexSpan(0, 5));
    CHECK(layer.token_view()->text(0) == "Howdy");
  }

  SUBCASE("falls back to span in plain text") {
    layers::TokenLayer layer("tokens", &plain_text);
    layer.tokens.emplace_back(layers::IndexSpan(0, 5));
    layer.tokens.emplace_back(layers::IndexSpan(6, 11));
    layer.tokens.emplace_back(layers::IndexSpan(11, 12));
    auto view = layer.token_view();
    CHECK(view->text(0) == "Hello");
    CHECK(view->text(1) == "world");
    CHECK(view->text(2) == "!");
  }

  SUBCASE("span view points into plain text") {
    layers::TokenLayer layer("tokens", &plain_text);
    layer.tokens.emplace_back(layers::IndexSpan(6, 11));
    CHECK(layer.token_view()->text(0).data() == plain_text.text.data() + 6);
  }

  SUBCASE("span without plain text gives empty text") {
    layers::TokenLayer layer;
    layer.tokens.emplace_back(layers::IndexSpan(0, 5));
    CHECK(layer.token_view()->text(0).empty());
  }

  SUBCASE("empty token gives empty text") {
    layers::TokenLayer layer("tokens", &plain_text);
    layer.tokens.emplace_back();
    CHECK(layer.token_view()->text(0).empty());
  }

  SUBCASE("span out of bounds throws") {
    layers::TokenLayer layer("tokens", &plain_text);
    layer.tokens.emplace_back(layers::IndexSpan(6, 13));
    layer.tokens.emplace_back(layers::IndexSpan(-1, 3));
    auto view = layer.token_view();
    CHECK_THROWS_AS(view->text(0), LinpipeError);
    CHECK_THROWS_AS(view->text(1), LinpipeError);
  }

  SUBCASE("span ending exactly at end of text is valid") {
    layers::TokenLayer layer("tokens", &plain_text);
    layer.tokens.emplace_back(layers::IndexSpan(0, 12));
    CHECK(layer.token_view()->text(0) == "Hello world!");
  }
}

TEST_CASE("TokenLayerSentenceView") {
  using Spans = std::vector<layers::IndexSpan>;

  SUBCASE("no tokens means no sentences") {
    CHECK(normalize(0, {}).empty());
  }

  SUBCASE("keeps sentences covering all tokens") {
    CHECK(normalize(3, {{0, 2}, {2, 3}}) == Spans{{0, 2}, {2, 3}});
  }

  SUBCASE("no sentences means one sentence") {
    CHECK(normalize(3, {}) == Spans{{0, 3}});
  }

  SUBCASE("tokens outside sentences form implicit sentences") {
    CHECK(normalize(6, {{1, 2}, {4, 5}}) == Spans{{0, 1}, {1, 2}, {2, 4}, {4, 5}, {5, 6}});
  }

  SUBCASE("skips empty sentences") {
    CHECK(normalize(2, {{0, 0}, {0, 1}, {1, 1}}) == Spans{{0, 1}, {1, 2}});
  }

  SUBCASE("rejects invalid sentences") {
    for (auto&& invalid : {Spans{{-1, 1}}, Spans{{2, 1}}, Spans{{0, 4}}, Spans{{0, 2}, {1, 3}}, Spans{{2, 3}, {0, 1}}})
      CHECK_THROWS_AS(normalize(3, invalid), LinpipeError);
  }
}

TEST_CASE("TokenLayer::sentence_view and TokenViewSlice") {
  layers::TokenLayer layer;
  for (auto&& token : {"a", "b", "c", "d", "e"})
    layer.tokens.emplace_back(token);
  layer.sentences = {{0, 2}, {2, 5}};
  auto tokens = layer.token_view();

  SUBCASE("sentence view reflects the layer") {
    CHECK(spans(*layer.sentence_view()) == std::vector<layers::IndexSpan>{{0, 2}, {2, 5}});
    layer.sentences = {{0, 3}};
    CHECK(spans(*layer.sentence_view()) == std::vector<layers::IndexSpan>{{0, 3}, {3, 5}});
  }

  SUBCASE("slice gives tokens of the span") {
    CHECK(texts(TokenViewSlice(*tokens, layers::IndexSpan(1, 4))) == std::vector<std::string>{"b", "c", "d"});
    CHECK(TokenViewSlice(*tokens, layers::IndexSpan(2, 2)).size() == 0);
    CHECK(texts(TokenViewSlice(*tokens, layers::IndexSpan(0, 5))) == texts(*tokens));
  }

  SUBCASE("iterates tokens by sentences") {
    auto sentences = layer.sentence_view();
    std::vector<std::vector<std::string>> result;
    for (size_t s = 0; s < sentences->size(); s++)
      result.push_back(texts(TokenViewSlice(*tokens, sentences->span(s))));
    CHECK(result == std::vector<std::vector<std::string>>{{"a", "b"}, {"c", "d", "e"}});
  }

  SUBCASE("slice rejects span out of range") {
    CHECK_THROWS_AS((void)TokenViewSlice(*tokens, layers::IndexSpan(-1, 2)), LinpipeError);
    CHECK_THROWS_AS((void)TokenViewSlice(*tokens, layers::IndexSpan(3, 2)), LinpipeError);
    CHECK_THROWS_AS((void)TokenViewSlice(*tokens, layers::IndexSpan(4, 6)), LinpipeError);
  }
}

TEST_CASE("Token JSON serialization") {
  SUBCASE("text-only token is a string") {
    CHECK(Json(layers::Token("Hello")) == Json("Hello"));
  }

  SUBCASE("span-only token is an object without text") {
    CHECK(Json(layers::Token(layers::IndexSpan(6, 11))) == Json::parse(R"({"span": [6, 11]})"));
  }

  SUBCASE("token with text and span has both") {
    CHECK(Json(layers::Token("world", layers::IndexSpan(6, 11))) == Json::parse(R"({"text": "world", "span": [6, 11]})"));
  }

  SUBCASE("parses all forms") {
    check_token(Json("Hello").get<layers::Token>(), "Hello", 0, 0);
    check_token(Json::parse(R"({"span": [6, 11]})").get<layers::Token>(), "", 6, 11);
    check_token(Json::parse(R"({"text": "!", "span": [11, 12]})").get<layers::Token>(), "!", 11, 12);
  }

  SUBCASE("rejects invalid tokens") {
    for (auto&& invalid : {R"(42)", R"(null)", R"([6, 11])", R"({})", R"({"text": ""})", R"({"text": 42})",
                           R"({"span": 6})", R"({"span": [6]})", R"({"span": [6, 11, 12]})", R"({"span": ["6", "11"]})",
                           R"({"span": [-1, 3]})", R"({"span": [5, 3]})"}) {
      CAPTURE(invalid);
      CHECK_THROWS_AS(Json::parse(invalid).get<layers::Token>(), LinpipeError);
    }
  }
}

TEST_CASE("TokenLayer::from_json") {
  layers::TokenLayer layer;

  SUBCASE("rejects invalid layers") {
    CHECK_THROWS_AS(layer.from_json(Json(42)), LinpipeError);
    CHECK_THROWS_AS(layer.from_json(Json{{"type", "token_layer"}, {"name", "tokens"}}), LinpipeError);
    CHECK_THROWS_AS(layer.from_json(layer_json("Hello")), LinpipeError);
    CHECK_THROWS_AS(layer.from_json(layer_json(Json::parse(R"(["Hello", 42])"))), LinpipeError);
    CHECK_THROWS_AS(layer.from_json(layer_json(Json::parse(R"(["Hello", {"span": [5, 3]}])"))), LinpipeError);
  }

  SUBCASE("loads mixed tokens and sentences") {
    Json json = layer_json(Json::parse(R"(["Hello", {"span": [6, 11]}, {"text": "!", "span": [11, 12]}])"));
    json["sentences"] = Json::parse("[[0, 3]]");
    layer.from_json(json);

    CHECK(layer.name() == "tokens");
    REQUIRE(layer.tokens.size() == 3);
    check_token(layer.tokens[0], "Hello", 0, 0);
    check_token(layer.tokens[1], "", 6, 11);
    check_token(layer.tokens[2], "!", 11, 12);
    CHECK(layer.sentences == std::vector<layers::IndexSpan>{{0, 3}});
  }

  SUBCASE("loads multiple sentences, possibly with gaps") {
    Json json = layer_json(Json::array({"a", "b", "c", "d"}));
    json["sentences"] = Json::parse("[[0, 2], [3, 4]]");
    layer.from_json(json);
    CHECK(layer.sentences == std::vector<layers::IndexSpan>{{0, 2}, {3, 4}});
  }

  SUBCASE("rejects invalid sentences") {
    for (auto&& invalid : {R"(3)", R"([3])", R"([[0]])", R"([[0, 1, 2]])", R"([["0", "1"]])", R"([[-1, 2]])",
                           R"([[2, 1]])", R"([[0, 4]])", R"([[0, 2], [1, 3]])", R"([[2, 3], [0, 1]])"}) {
      CAPTURE(invalid);
      Json json = layer_json(Json::array({"a", "b", "c"}));
      json["sentences"] = Json::parse(invalid);
      CHECK_THROWS_AS(layer.from_json(json), LinpipeError);
    }
  }

  SUBCASE("accepts empty tokens") {
    layer.from_json(layer_json(Json::array()));
    CHECK(layer.tokens.empty());
  }

  SUBCASE("replaces previous content") {
    layer.tokens.emplace_back("old");
    layer.sentences = {{0, 1}};
    layer.from_json(layer_json(Json::array({"new"})));
    REQUIRE(layer.tokens.size() == 1);
    CHECK(layer.tokens[0].text == "new");
    CHECK(layer.sentences.empty());
  }
}

TEST_CASE("TokenLayer::to_json") {
  layers::TokenLayer layer("tokens");
  layer.tokens.emplace_back("Hello");
  layer.tokens.emplace_back(layers::IndexSpan(6, 11));
  layer.tokens.emplace_back("!", layers::IndexSpan(11, 12));
  layer.sentences = {{0, 3}};

  SUBCASE("writes expected JSON") {
    CHECK(layer.to_json() == Json::parse(R"({
      "type": "token_layer",
      "name": "tokens",
      "tokens": ["Hello", {"span": [6, 11]}, {"text": "!", "span": [11, 12]}],
      "sentences": [[0, 3]]
    })"));
  }

  SUBCASE("round-trips through from_json") {
    layers::TokenLayer loaded;
    loaded.from_json(layer.to_json());

    CHECK(loaded.name() == layer.name());
    CHECK(loaded.sentences == layer.sentences);
    REQUIRE(loaded.tokens.size() == layer.tokens.size());
    for (size_t i = 0; i < layer.tokens.size(); i++) {
      CAPTURE(i);
      check_token(loaded.tokens[i], layer.tokens[i].text, layer.tokens[i].index_span.begin, layer.tokens[i].index_span.end);
    }
  }
}

} // namespace linpipe
