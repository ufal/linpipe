// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/doctest/doctest.h"
#include "operations/operation_description.h"

namespace linpipe {

namespace {

std::vector<OperationDescription> parse(std::initializer_list<std::string_view> tokens) {
  return OperationDescription::parse_descriptions(std::span<const std::string_view>(tokens.begin(), tokens.size()));
}

void check_description(const OperationDescription& description, std::string_view operation,
                       const std::vector<std::string>& args, const std::vector<std::pair<std::string, std::string>>& kwargs) {
  CHECK(description.operation == operation);
  CHECK(description.args == args);
  CHECK(description.kwargs.size() == kwargs.size());
  for (const auto& [key, value] : kwargs) {
    auto it = description.kwargs.find(key);
    REQUIRE(it != description.kwargs.end());
    CHECK(it->second == value);
  }
}

} // namespace

TEST_CASE("OperationDescription::parse_descriptions") {
  SUBCASE("parses empty token list") {
    CHECK(parse({}).empty());
  }

  SUBCASE("parses single operation without arguments") {
    auto descriptions = parse({"-load"});
    REQUIRE(descriptions.size() == 1);
    check_description(descriptions[0], "load", {}, {});
  }

  SUBCASE("parses two operations with named and positional arguments") {
    auto descriptions = parse({"-load", "--format", "text", "test.in", "-save", "--format=lif", "test.out"});
    REQUIRE(descriptions.size() == 2);
    check_description(descriptions[0], "load", {"test.in"}, {{"format", "text"}});
    check_description(descriptions[1], "save", {"test.out"}, {{"format", "lif"}});
  }

  SUBCASE("preserves order of interleaved positional arguments") {
    auto descriptions = parse({"-load", "first.in", "--format", "conll-2003", "second.in", "--batch_size", "32"});
    REQUIRE(descriptions.size() == 1);
    check_description(descriptions[0], "load", {"first.in", "second.in"}, {{"format", "conll-2003"}, {"batch_size", "32"}});
  }

  SUBCASE("later named argument wins") {
    auto descriptions = parse({"-load", "--format", "text", "--format=lif"});
    REQUIRE(descriptions.size() == 1);
    check_description(descriptions[0], "load", {}, {{"format", "lif"}});
  }

  SUBCASE("accepts negative numbers as values and positional arguments") {
    auto descriptions = parse({"-tag", "--threshold", "-1", "--margin=-2", "--scale", "-0.5", "--offset", "-.5", "-3", "--name", "---x"});
    REQUIRE(descriptions.size() == 1);
    check_description(descriptions[0], "tag", {"-3"}, {{"threshold", "-1"}, {"margin", "-2"}, {"scale", "-0.5"}, {"offset", "-.5"}, {"name", "---x"}});
  }

  SUBCASE("accepts operation-like and argument-like values") {
    auto descriptions = parse({"-load", "--name=-value", "--other", "--value"});
    REQUIRE(descriptions.size() == 1);
    check_description(descriptions[0], "load", {}, {{"name", "-value"}, {"other", "--value"}});
  }

  SUBCASE("splits on the first '=' only and allows empty values") {
    auto descriptions = parse({"-load", "--format=conll(1=name:type,4_encoding=bio)", "--title=", "--note", "a=b"});
    REQUIRE(descriptions.size() == 1);
    check_description(descriptions[0], "load", {}, {{"format", "conll(1=name:type,4_encoding=bio)"}, {"title", ""}, {"note", "a=b"}});
  }

  SUBCASE("treats '---...' and empty tokens as positional arguments") {
    auto descriptions = parse({"-load", "---", "---x", ""});
    REQUIRE(descriptions.size() == 1);
    check_description(descriptions[0], "load", {"---", "---x", ""}, {});
  }

  SUBCASE("accepts '-' (stdin) as positional argument and as value") {
    auto descriptions = parse({"-load", "-", "--input", "-", "--output=-", "-save", "-"});
    REQUIRE(descriptions.size() == 2);
    check_description(descriptions[0], "load", {"-"}, {{"input", "-"}, {"output", "-"}});
    check_description(descriptions[1], "save", {"-"}, {});
  }

  SUBCASE("treats tokens between '--' and '--' as positional arguments") {
    auto descriptions = parse({"-load", "--format", "text", "in.txt", "--", "-save", "--format=lif", "-", "-1", "--", "--batch_size", "32", "-save", "--format=lif", "out.txt"});
    REQUIRE(descriptions.size() == 2);
    check_description(descriptions[0], "load", {"in.txt", "-save", "--format=lif", "-", "-1"}, {{"format", "text"}, {"batch_size", "32"}});
    check_description(descriptions[1], "save", {"out.txt"}, {{"format", "lif"}});
  }

  SUBCASE("accepts '--' at the end of the pipeline") {
    auto descriptions = parse({"-load", "in.txt", "--"});
    REQUIRE(descriptions.size() == 1);
    check_description(descriptions[0], "load", {"in.txt"}, {});
  }

  SUBCASE("accepts '--' as a value of a named argument") {
    auto descriptions = parse({"-load", "--separator", "--", "-save"});
    REQUIRE(descriptions.size() == 2);
    check_description(descriptions[0], "load", {}, {{"separator", "--"}});
    check_description(descriptions[1], "save", {}, {});
  }

  SUBCASE("throws when the pipeline does not start with an operation") {
    CHECK_THROWS_AS(parse({"test.in", "-load"}), LinpipeError);
    CHECK_THROWS_AS(parse({"--load"}), LinpipeError);
    CHECK_THROWS_AS(parse({"--format", "text", "-load"}), LinpipeError);
    CHECK_THROWS_AS(parse({"-1", "-load"}), LinpipeError);
    CHECK_THROWS_AS(parse({"-", "-load"}), LinpipeError);
    CHECK_THROWS_AS(parse({"--", "-load"}), LinpipeError);
  }
}

TEST_CASE("OperationDescription::tokenize") {
  using Tokens = std::vector<std::string>;

  SUBCASE("tokenizes empty and whitespace-only description") {
    CHECK(OperationDescription::tokenize("").empty());
    CHECK(OperationDescription::tokenize(" \t\n\r ").empty());
  }

  SUBCASE("splits on any whitespace") {
    CHECK(OperationDescription::tokenize("  -load\t--format\ntext\r\ntest.in ") == Tokens{"-load", "--format", "text", "test.in"});
  }

  SUBCASE("groups quoted whitespace into one token") {
    CHECK(OperationDescription::tokenize("-load \"/tmp/my file.txt\"") == Tokens{"-load", "/tmp/my file.txt"});
  }

  SUBCASE("handles quotes inside a token") {
    CHECK(OperationDescription::tokenize("--title=\"a b\" a\"b c\"d") == Tokens{"--title=a b", "ab cd"});
  }

  SUBCASE("handles escapes inside and outside quotes") {
    auto description = R"("say \"hi\"" "a\\b" a\"b)"sv;  // factored-out to sidestep VC++ escaped-strings-in-stringize bug
    CHECK(OperationDescription::tokenize(description) == Tokens{"say \"hi\"", "a\\b", "a\"b"});
  }

  SUBCASE("keeps other backslashes literal") {
    auto description = R"(C:\dir "C:\dir" C:\dir\)"sv;  // factored-out to sidestep VC++ escaped-strings-in-stringize bug
    CHECK(OperationDescription::tokenize(description) == Tokens{"C:\\dir", "C:\\dir", "C:\\dir\\"});
  }

  SUBCASE("keeps empty quoted tokens") {
    CHECK(OperationDescription::tokenize("\"\" a \"\"") == Tokens{"", "a", ""});
  }

  SUBCASE("groups single-quoted whitespace into one token") {
    CHECK(OperationDescription::tokenize("-load '/tmp/my file.txt'") == Tokens{"-load", "/tmp/my file.txt"});
  }
}

TEST_CASE("OperationDescription tokenize + parse_descriptions") {
  auto tokens = OperationDescription::tokenize(R"(-load "/tmp/my file.txt" --title "a -b" -save --title="c -d" out.txt)");
  std::vector<std::string_view> views(tokens.begin(), tokens.end());
  auto descriptions = OperationDescription::parse_descriptions(views);
  REQUIRE(descriptions.size() == 2);
  check_description(descriptions[0], "load", {"/tmp/my file.txt"}, {{"title", "a -b"}});
  check_description(descriptions[1], "save", {"out.txt"}, {{"title", "c -d"}});
}

} // namespace linpipe
