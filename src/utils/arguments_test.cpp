// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <unordered_map>

#include "lib/doctest.h"
#include "utils/arguments.h"

namespace linpipe {

TEST_CASE("Arguments::parse_operations") {
  Arguments args;
  std::vector<std::string> parsed;
  std::vector<std::string> gold;

  SUBCASE("parses single operation name") {
    gold.push_back(" --load");
    CHECK_NOTHROW(args.parse_operations(parsed, " --load"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses single operation with argument") {
    gold.push_back(" --load -format text");
    CHECK_NOTHROW(args.parse_operations(parsed, " --load -format text"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses two operations") {
    gold.push_back(" --load");
    gold.push_back(" --save");
    CHECK_NOTHROW(args.parse_operations(parsed, " --load --save"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses two operations with arguments") {
    gold.push_back(" --load -format text");
    gold.push_back(" --save -format text");
    CHECK_NOTHROW(args.parse_operations(parsed, " --load -format text --save -format text"));
    CHECK(parsed == gold);
  }

  SUBCASE("parses two operations with arguments and kwargs") {
    gold.push_back(" --load -format text test.in");
    gold.push_back(" --save -format lif test.out");
    CHECK_NOTHROW(args.parse_operations(parsed, " --load -format text test.in --save -format lif test.out"));
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
    gold_kwargs.push_back("test.in");
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
    gold_kwargs.push_back("dummy");
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --load -format lif dummy"));
    CHECK(gold_args == args);
    CHECK(gold_kwargs == kwargs);
  }

  SUBCASE("accepts negative number as arg value") {
    gold_args["threshold"] = "-1";
    CHECK_NOTHROW(parser.parse_arguments(args, kwargs, " --tag -threshold -1"));
    CHECK(gold_args == args);
    CHECK(kwargs.empty());
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
