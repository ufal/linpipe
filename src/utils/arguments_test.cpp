// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <unordered_map>

#include "lib/doctest/doctest.h"
#include "utils/arguments.h"

namespace linpipe {

TEST_CASE("Arguments::parse_operations") {
  Arguments args;
  std::vector<std::string> parsed;
  std::vector<std::string> gold;

  SUBCASE("parses empty description") {
    CHECK_NOTHROW(args.parse_operations(parsed, ""));
    CHECK(parsed.empty());
  }

  SUBCASE("parses single operation name") {
    gold = {" --load"};
    CHECK_NOTHROW(args.parse_operations(parsed, " --load"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses description without leading space") {
    gold = {" --load -format text", " --save"};
    CHECK_NOTHROW(args.parse_operations(parsed, "--load -format text --save"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses single operation with argument") {
    gold = {" --load -format text"};
    CHECK_NOTHROW(args.parse_operations(parsed, " --load -format text"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses two operations") {
    gold = {" --load", " --save"};
    CHECK_NOTHROW(args.parse_operations(parsed, " --load --save"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses two operations with arguments and kwargs") {
    gold = {" --load -format text test.in", " --save -format=lif test.out"};
    CHECK_NOTHROW(args.parse_operations(parsed, " --load -format text test.in --save -format=lif test.out"));
    CHECK(parsed == gold);
  }

  SUBCASE("normalizes whitespace between tokens") {
    gold = {" --load -format text", " --save"};
    CHECK_NOTHROW(args.parse_operations(parsed, "  --load\t-format   text\n --save  "));
    CHECK(parsed == gold);
  }

  SUBCASE("keeps quoted values with spaces and ' --' in one operation") {
    gold = {" --load \"/tmp/my file.txt\" -title \"a --b\"", " --save \"-title=c --d\""};
    CHECK_NOTHROW(args.parse_operations(parsed, " --load \"/tmp/my file.txt\" -title \"a --b\" --save -title=\"c --d\""));
    CHECK(parsed == gold);
  }

  SUBCASE("does not treat '---' as operation") {
    gold = {" --load ---"};
    CHECK_NOTHROW(args.parse_operations(parsed, " --load ---"));
    CHECK(parsed == gold);
  }

  SUBCASE("throws on single-hyphen operation") {
    CHECK_THROWS_AS(args.parse_operations(parsed, " -load"), LinpipeError);
  }

  SUBCASE("throws when description does not start with operation") {
    CHECK_THROWS_AS(args.parse_operations(parsed, " test.in --load"), LinpipeError);
  }

  SUBCASE("throws on '--' without operation name") {
    CHECK_THROWS_AS(args.parse_operations(parsed, " --load --"), LinpipeError);
    CHECK_THROWS_AS(args.parse_operations(parsed, " --load -- test.in"), LinpipeError);
  }

  SUBCASE("throws on missing closing quote") {
    CHECK_THROWS_AS(args.parse_operations(parsed, " --load \"test.in"), LinpipeError);
  }
}

TEST_CASE("Arguments::parse_arguments") {
  Arguments parser;
  std::unordered_map<std::string, std::string> args;
  std::vector<std::string> kwargs;
  std::unordered_map<std::string, std::string> gold_args;
  std::vector<std::string> gold_kwargs;

  SUBCASE("parses operation without arguments") {
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load"));
    CHECK(args.empty());
    CHECK(kwargs.empty());
  }

  SUBCASE("parses 1 kwarg in 1 operation") {
    gold_kwargs = {"test.in"};
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load test.in"));
    CHECK(args.empty());
    CHECK(gold_kwargs == kwargs);
  }

  SUBCASE("parses 1 arg in 1 operation") {
    gold_args["format"] = "text";
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load -format text"));
    CHECK(gold_args == args);
    CHECK(kwargs.empty());
  }

  SUBCASE("parses 1 arg and 1 kwargs in 1 operation") {
    gold_args["format"] = "lif";
    gold_kwargs = {"dummy"};
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load -format lif dummy"));
    CHECK(gold_args == args);
    CHECK(gold_kwargs == kwargs);
  }

  SUBCASE("parses 2 args and 2 kwargs in 1 operation") {
    gold_args["format"] = "conll-2003";
    gold_args["batch_size"] = "32";
    gold_kwargs = {"first.in", "second.in"};
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load first.in -format conll-2003 second.in -batch_size 32"));
    CHECK(gold_args == args);
    CHECK(gold_kwargs == kwargs);
  }

  SUBCASE("accepts negative number as arg value") {
    gold_args["threshold"] = "-1";
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --tag -threshold -1"));
    CHECK(gold_args == args);
    CHECK(kwargs.empty());
  }

  SUBCASE("parses arg with value after '='") {
    gold_args["format"] = "text";
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load -format=text"));
    CHECK(gold_args == args);
    CHECK(kwargs.empty());
  }

  SUBCASE("parses args with and without '=' and kwargs") {
    gold_args["format"] = "text";
    gold_args["batch_size"] = "32";
    gold_kwargs = {"test.in"};
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load -format=text test.in -batch_size 32"));
    CHECK(gold_args == args);
    CHECK(gold_kwargs == kwargs);
  }

  SUBCASE("splits arg on first '=' only") {
    gold_args["format"] = "conll(1=name:type,4_encoding=bio)";
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load -format=conll(1=name:type,4_encoding=bio)"));
    CHECK(gold_args == args);
    CHECK(kwargs.empty());
  }

  SUBCASE("keeps '=' in value given as next token") {
    gold_args["format"] = "conll(1=name:type)";
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load -format conll(1=name:type)"));
    CHECK(gold_args == args);
    CHECK(kwargs.empty());
  }

  SUBCASE("parses empty value after '='") {
    gold_args["format"] = "";
    gold_kwargs = {"test.in"};
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load -format= test.in"));
    CHECK(gold_args == args);
    CHECK(gold_kwargs == kwargs);
  }

  SUBCASE("accepts negative number as arg value after '='") {
    gold_args["threshold"] = "-1";
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --tag -threshold=-1"));
    CHECK(gold_args == args);
    CHECK(kwargs.empty());
  }

  SUBCASE("parses quoted values with spaces") {
    gold_args["title"] = "my title";
    gold_args["note"] = "a = b";
    gold_kwargs = {"/tmp/my file.txt"};
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load \"/tmp/my file.txt\" -title \"my title\" -note=\"a = b\""));
    CHECK(gold_args == args);
    CHECK(gold_kwargs == kwargs);
  }

  SUBCASE("parses quoted value with ' --'") {
    gold_args["title"] = " a --b";
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load -title \" a --b\""));
    CHECK(gold_args == args);
    CHECK(kwargs.empty());
  }

  SUBCASE("parses escaped quotes and backslashes") {
    gold_args["title"] = "say \"hi\"";
    gold_kwargs = {"C:\\dir\\file.txt"};
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load -title \"say \\\"hi\\\"\" \"C:\\\\dir\\\\file.txt\""));
    CHECK(gold_args == args);
    CHECK(gold_kwargs == kwargs);
  }

  SUBCASE("parses empty quoted tokens") {
    gold_args["title"] = "";
    gold_kwargs = {""};
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load \"\" -title \"\""));
    CHECK(gold_args == args);
    CHECK(gold_kwargs == kwargs);
  }

  SUBCASE("throws on '=' without arg name") {
    CHECK_THROWS_AS(parser.parse_arguments(args, kwargs, " --load -=text"), LinpipeError);
  }

  SUBCASE("throws on arg without value") {
    CHECK_THROWS_AS(parser.parse_arguments(args, kwargs, " --load -format"), LinpipeError);
    CHECK_THROWS_AS(parser.parse_arguments(args, kwargs, " --load test.in -format"), LinpipeError);
  }

  SUBCASE("throws when description does not start with operation") {
    CHECK_THROWS_AS(parser.parse_arguments(args, kwargs, ""), LinpipeError);
    CHECK_THROWS_AS(parser.parse_arguments(args, kwargs, " test.in"), LinpipeError);
    CHECK_THROWS_AS(parser.parse_arguments(args, kwargs, " -load test.in"), LinpipeError);
  }

  SUBCASE("throws on missing closing quote") {
    CHECK_THROWS_AS(parser.parse_arguments(args, kwargs, " --load -title \"my title"), LinpipeError);
  }
}

TEST_CASE("Arguments::tokenize") {
  std::vector<std::string> tokens;
  std::vector<std::string> gold;

  SUBCASE("tokenizes empty and whitespace-only description") {
    CHECK_NOTHROW(Arguments::tokenize(tokens, ""));
    CHECK(tokens.empty());
    CHECK_NOTHROW(Arguments::tokenize(tokens, " \t\n "));
    CHECK(tokens.empty());
  }

  SUBCASE("splits on any whitespace") {
    gold = {"--load", "-format", "text", "test.in"};
    CHECK_NOTHROW(Arguments::tokenize(tokens, "  --load\t-format\ntext  test.in "));
    CHECK(tokens == gold);
  }

  SUBCASE("groups quoted whitespace into one token") {
    gold = {"--load", "/tmp/my file.txt"};
    CHECK_NOTHROW(Arguments::tokenize(tokens, "--load \"/tmp/my file.txt\""));
    CHECK(tokens == gold);
  }

  SUBCASE("handles quotes inside a token") {
    gold = {"-title=a b", "ab cd"};
    CHECK_NOTHROW(Arguments::tokenize(tokens, "-title=\"a b\" a\"b c\"d"));
    CHECK(tokens == gold);
  }

  SUBCASE("handles escapes inside and outside quotes") {
    gold = {"say \"hi\"", "a\\b", "a\"b"};
    CHECK_NOTHROW(Arguments::tokenize(tokens, "\"say \\\"hi\\\"\" \"a\\\\b\" a\\\"b"));
    CHECK(tokens == gold);
  }

  SUBCASE("keeps other backslashes literal") {
    gold = {"C:\\dir", "C:\\dir"};
    CHECK_NOTHROW(Arguments::tokenize(tokens, "C:\\dir \"C:\\dir\""));
    CHECK(tokens == gold);
  }

  SUBCASE("keeps empty quoted tokens") {
    gold = {"", "a", ""};
    CHECK_NOTHROW(Arguments::tokenize(tokens, "\"\" a \"\""));
    CHECK(tokens == gold);
  }

  SUBCASE("throws on missing closing quote") {
    CHECK_THROWS_AS(Arguments::tokenize(tokens, "\"abc"), LinpipeError);
    CHECK_THROWS_AS(Arguments::tokenize(tokens, "a \"b c"), LinpipeError);
    CHECK_THROWS_AS(Arguments::tokenize(tokens, "\"abc\\\""), LinpipeError);
  }
}

TEST_CASE("Arguments::join") {
  CHECK(Arguments::join({}) == "");
  CHECK(Arguments::join({"--load", "-format", "text"}) == "--load -format text");
  CHECK(Arguments::join({"--load", "/tmp/my file.txt"}) == "--load \"/tmp/my file.txt\"");
  CHECK(Arguments::join({"--load", ""}) == "--load \"\"");
  CHECK(Arguments::join({"-title", "say \"hi\"", "a\\b"}) == "-title \"say \\\"hi\\\"\" \"a\\\\b\"");

  SUBCASE("is inverse of tokenize") {
    std::vector<std::vector<std::string>> cases = {
      {},
      {"--load", "-format", "text", "test.in"},
      {"--load", "/tmp/my file.txt", "-title=a b", "a --b"},
      {"", " ", "\t\n", "\"", "\\", "\\\"", "a\\", "\"quoted\"", "C:\\dir\\"},
    };
    for (const auto& gold : cases) {
      std::vector<std::string> tokens;
      CHECK_NOTHROW(Arguments::tokenize(tokens, Arguments::join(gold)));
      CHECK(tokens == gold);
    }
  }
}

TEST_CASE("Arguments::parse_format") {
  Arguments args;
  std::unordered_map<std::string, std::string> parsed;
  std::unordered_map<std::string, std::string> gold;

  SUBCASE("parses empty description") {
    CHECK_NOTHROW(args.parse_format(parsed, ""));
    CHECK(parsed == gold);
  }

  SUBCASE("parses one key-value pair") {
    CHECK_NOTHROW(args.parse_format(parsed, "key=value"));
    gold["key"] = "value";
    CHECK(parsed == gold);
  }

  SUBCASE("parses CoNLL-2003") {
    CHECK_NOTHROW(args.parse_format(parsed, "conll(1=name:type,2=:lemmas,2_default=_,3=:chunks,3_default=_,4=:named_entities,4_encoding=bio)"));
    gold["1"] = "name:type";
    gold["2"] = ":lemmas";
    gold["2_default"] = "_";
    gold["3"] = ":chunks";
    gold["3_default"] = "_";
    gold["4"] = ":named_entities";
    gold["4_encoding"] = "bio";
    CHECK(parsed == gold);
  }
}

} // namespace linpipe
