// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <sstream>

#include "core/document.h"
#include "formats/conll.h"
#include "layers/lemmas.h"
#include "layers/plain_text.h"
#include "layers/spans.h"
#include "layers/token_layer.h"
#include "lib/doctest/doctest.h"

namespace linpipe {

namespace {

std::unique_ptr<Document> load(const std::string& description, const std::string& input) {
  std::unique_ptr<Format> format = Format::create(description);
  std::istringstream is(input);
  return format->load(is, "source");
}

std::string save(const std::string& description, Document& document) {
  std::unique_ptr<Format> format = Format::create(description);
  std::ostringstream os;
  format->save(document, os);
  return os.str();
}

std::string round_trip(const std::string& description, const std::string& input) {
  std::unique_ptr<Format> format = Format::create(description);
  std::istringstream is(input);
  auto document = format->load(is, "source");
  std::ostringstream os;
  format->save(*document, os);
  return os.str();
}

std::vector<std::string> texts(layers::TokenLayer& layer) {
  std::vector<std::string> result;
  auto view = layer.token_view();
  for (size_t i = 0; i < view->size(); i++)
    result.emplace_back(view->text(i));
  return result;
}

} // namespace

TEST_CASE("formats::Conll::Conll") {
  SUBCASE("accepts supported column types") {
    CHECK_NOTHROW(Format::create("conll"));
    CHECK_NOTHROW(Format::create("conll(1=token_layer,2=lemmas,3=spans)"));
    CHECK_NOTHROW(Format::create("conll(1=form:token_layer,2=ner:spans,2_encoding=BIO)"));
  }

  SUBCASE("rejects unsupported column type") {
    CHECK_THROWS_AS(Format::create("conll(1=plain_text)"), LinpipeError);
    CHECK_THROWS_AS(Format::create("conll(1=token_layer,2=nonsense)"), LinpipeError);
  }

  SUBCASE("rejects unknown span encoding") {
    CHECK_THROWS_AS(Format::create("conll(1=token_layer,2=spans,2_encoding=XYZ)"), LinpipeError);
  }
}

TEST_CASE("formats::Conll::load") {
  SUBCASE("loads tokens and sentences with default layer name") {
    auto doc = load("conll", "Hello\nworld\n\nBye\n\n");
    CHECK(doc->source_path() == "source");
    REQUIRE(doc->layers().size() == 1);
    auto& layer = doc->get_layer<layers::TokenLayer>("token_layer");
    CHECK(texts(layer) == std::vector<std::string>{"Hello", "world", "Bye"});
    CHECK(layer.sentences == std::vector<layers::IndexSpan>{{0, 2}, {2, 3}});
  }

  SUBCASE("loaded tokens are text-only") {
    auto doc = load("conll", "Hello\n\n");
    auto& token = doc->get_layer<layers::TokenLayer>().tokens.at(0);
    CHECK(token.text == "Hello");
    CHECK(token.index_span.empty());
  }

  SUBCASE("uses layer names from the description") {
    auto doc = load("conll(1=form:token_layer,2=lemma:lemmas)", "cats\tcat\n\n");
    CHECK(doc->get_layer<layers::TokenLayer>("form").tokens.size() == 1);
    CHECK(doc->get_layer<layers::Lemmas>("lemma").lemmas == std::vector<std::string>{"cat"});
  }

  SUBCASE("loads lemmas and BIO spans") {
    auto doc = load("conll(1=token_layer,2=lemmas,3=spans)",
                    "John\tJohn\tB-PER\nSmith\tSmith\tI-PER\nlives\tlive\tO\nin\tin\tO\nPrague\tPrague\tB-LOC\n\n");
    CHECK(doc->get_layer<layers::Lemmas>().lemmas == std::vector<std::string>{"John", "Smith", "live", "in", "Prague"});
    auto& spans = doc->get_layer<layers::Spans>();
    CHECK(spans.spans == std::vector<std::pair<unsigned, unsigned>>{{0, 1}, {4, 4}});
    CHECK(spans.tags == std::vector<std::string>{"PER", "LOC"});
  }

  SUBCASE("records last sentence without trailing empty line") {
    auto doc = load("conll", "a\nb\n\nc");
    CHECK(doc->get_layer<layers::TokenLayer>().sentences == std::vector<layers::IndexSpan>{{0, 2}, {2, 3}});
  }

  SUBCASE("ignores repeated and leading empty lines") {
    auto doc = load("conll", "\n\na\n\n\n\nb\n\n\n");
    CHECK(doc->get_layer<layers::TokenLayer>().sentences == std::vector<layers::IndexSpan>{{0, 1}, {1, 2}});
  }

  SUBCASE("handles CRLF line endings") {
    auto doc = load("conll(1=token_layer,2=lemmas)", "cats\tcat\r\n\r\n");
    CHECK(texts(doc->get_layer<layers::TokenLayer>()) == std::vector<std::string>{"cats"});
    CHECK(doc->get_layer<layers::Lemmas>().lemmas == std::vector<std::string>{"cat"});
    CHECK(doc->get_layer<layers::TokenLayer>().sentences == std::vector<layers::IndexSpan>{{0, 1}});
  }

  SUBCASE("rejects wrong number of columns") {
    CHECK_THROWS_AS(load("conll(1=token_layer,2=lemmas)", "cats\n\n"), LinpipeError);
    CHECK_THROWS_AS(load("conll(1=token_layer,2=lemmas)", "cats\tcat\textra\n\n"), LinpipeError);
  }
}

TEST_CASE("formats::Conll::save") {
  SUBCASE("prints an empty line after every sentence") {
    Document doc;
    auto& layer = static_cast<layers::TokenLayer&>(doc.add_layer(std::make_unique<layers::TokenLayer>()));
    for (auto&& token : {"Hello", "world", "Bye"})
      layer.tokens.emplace_back(token);
    layer.sentences = {{0, 2}, {2, 3}};
    CHECK(save("conll", doc) == "Hello\nworld\n\nBye\n\n");
  }

  SUBCASE("prints tokens outside sentences as separate sentences") {
    Document doc;
    auto& layer = static_cast<layers::TokenLayer&>(doc.add_layer(std::make_unique<layers::TokenLayer>()));
    for (auto&& token : {"a", "b", "c", "d"})
      layer.tokens.emplace_back(token);
    layer.sentences = {{1, 3}};
    CHECK(save("conll", doc) == "a\n\nb\nc\n\nd\n\n");
  }

  SUBCASE("rejects sentences out of token range") {
    Document doc;
    auto& layer = static_cast<layers::TokenLayer&>(doc.add_layer(std::make_unique<layers::TokenLayer>()));
    layer.tokens.emplace_back("a");
    layer.sentences = {{0, 2}};
    CHECK_THROWS_AS(save("conll", doc), LinpipeError);
    layer.sentences = {{-1, 1}};
    CHECK_THROWS_AS(save("conll", doc), LinpipeError);
  }

  SUBCASE("treats a document without sentences as one sentence") {
    Document doc;
    auto& layer = static_cast<layers::TokenLayer&>(doc.add_layer(std::make_unique<layers::TokenLayer>()));
    layer.tokens.emplace_back("a");
    layer.tokens.emplace_back("b");
    CHECK(save("conll", doc) == "a\nb\n\n");
  }

  SUBCASE("prints nothing for an empty document") {
    Document doc;
    doc.add_layer(std::make_unique<layers::TokenLayer>());
    CHECK(save("conll", doc) == "");
  }

  SUBCASE("prints tokens anchored in plain text") {
    Document doc;
    auto& plain_text = static_cast<layers::PlainText&>(doc.add_layer(std::make_unique<layers::PlainText>()));
    plain_text.text = "Hello world!";
    auto& layer = static_cast<layers::TokenLayer&>(doc.add_layer(std::make_unique<layers::TokenLayer>("tokens", &plain_text)));
    layer.tokens.emplace_back(layers::IndexSpan(0, 5));
    layer.tokens.emplace_back("World", layers::IndexSpan(6, 11));  // explicit text overrides the anchor
    layer.tokens.emplace_back(layers::IndexSpan(11, 12));
    CHECK(save("conll(1=tokens:token_layer)", doc) == "Hello\nWorld\n!\n\n");
  }

  SUBCASE("prints token layer that is not the first layer or column") {
    Document doc;
    doc.add_layer(std::make_unique<layers::PlainText>());
    auto& lemmas = static_cast<layers::Lemmas&>(doc.add_layer(std::make_unique<layers::Lemmas>()));
    auto& tokens = static_cast<layers::TokenLayer&>(doc.add_layer(std::make_unique<layers::TokenLayer>()));
    lemmas.lemmas = {"cat", "sleep"};
    tokens.tokens.emplace_back("cats");
    tokens.tokens.emplace_back("sleep");
    tokens.sentences = {{0, 1}, {1, 2}};
    CHECK(save("conll(1=lemmas,2=token_layer)", doc) == "cat\tcats\n\nsleep\tsleep\n\n");
  }

  SUBCASE("rejects columns of different lengths") {
    Document doc;
    auto& tokens = static_cast<layers::TokenLayer&>(doc.add_layer(std::make_unique<layers::TokenLayer>()));
    auto& lemmas = static_cast<layers::Lemmas&>(doc.add_layer(std::make_unique<layers::Lemmas>()));
    tokens.tokens.emplace_back("cats");
    tokens.tokens.emplace_back("sleep");
    lemmas.lemmas = {"cat"};
    CHECK_THROWS_AS(save("conll(1=token_layer,2=lemmas)", doc), LinpipeError);
  }

  SUBCASE("rejects spans out of token range") {
    Document doc;
    auto& tokens = static_cast<layers::TokenLayer&>(doc.add_layer(std::make_unique<layers::TokenLayer>()));
    auto& spans = static_cast<layers::Spans&>(doc.add_layer(std::make_unique<layers::Spans>()));
    tokens.tokens.emplace_back("Prague");
    spans.spans = {{0, 1}};
    spans.tags = {"LOC"};
    CHECK_THROWS_AS(save("conll(1=token_layer,2=spans)", doc), LinpipeError);
  }

  SUBCASE("rejects format with only span columns") {
    Document doc;
    doc.add_layer(std::make_unique<layers::Spans>());
    CHECK_THROWS_AS(save("conll(1=spans)", doc), LinpipeError);
  }
}

TEST_CASE("formats::Conll round trip") {
  SUBCASE("tokens only") {
    std::string input = "Hello\nworld\n\nBye\n\n";
    CHECK(round_trip("conll", input) == input);
  }

  SUBCASE("tokens, lemmas and spans") {
    std::string input = "John\tJohn\tB-PER\nSmith\tSmith\tI-PER\nlives\tlive\tO\n\nin\tin\tO\nPrague\tPrague\tB-LOC\n\n";
    CHECK(round_trip("conll(1=token_layer,2=lemmas,3=spans)", input) == input);
  }

  SUBCASE("normalizes empty lines") {
    CHECK(round_trip("conll", "\na\n\n\nb") == "a\n\nb\n\n");
  }
}

} // namespace linpipe
