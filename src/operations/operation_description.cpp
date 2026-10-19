// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "operations/operation_description.h"

namespace linpipe {

namespace {

bool is_operation(std::string_view token) {
  return token.size() > 1 && token[0] == '-' && token[1] != '-' && token[1] != '.' && (token[1] < '0' || token[1] > '9');
}

} // namespace

std::vector<OperationDescription> OperationDescription::parse_descriptions(std::span<const std::string_view> tokens) {
  std::vector<OperationDescription> descriptions;
  bool only_positional = false;

  for (size_t i = 0; i < tokens.size(); i++) {
    std::string_view token = tokens[i];

    // after "--", all remaining tokens are positional arguments of the current operation
    if (only_positional) {
      descriptions.back().args.emplace_back(token);
      continue;
    }

    if (is_operation(token)) {
      if (token.find('=') != std::string_view::npos)
        throw LinpipeError{"OperationDescription::parse_descriptions: Operation name cannot contain '=' in '", token, "'; named arguments are written as '--name=value'"};

      descriptions.push_back({.operation = std::string{token.substr(1)}, .args = {}, .kwargs = {}});
      continue;
    }

    if (descriptions.empty())
      throw LinpipeError{"OperationDescription::parse_descriptions: The pipeline must start with an operation, but found '", token, "'"};

    OperationDescription& description = descriptions.back();

    if (token == "--") {
      only_positional = true;
      continue;
    }

    // named argument
    if (token.size() > 2 && token[0] == '-' && token[1] == '-' && token[2] != '-') {
      size_t eq = token.find('=');
      if (eq == std::string_view::npos) {
        if (i + 1 >= tokens.size() || is_operation(tokens[i + 1]))
          throw LinpipeError{"OperationDescription::parse_descriptions: Value expected for argument '", token, "' in operation '-", description.operation, "'"};

        description.kwargs.insert_or_assign(std::string{token.substr(2)}, std::string{tokens[++i]});
      } else {
        if (eq == 2)
          throw LinpipeError{"OperationDescription::parse_descriptions: Argument name expected before '=' in '", token, "' in operation '-", description.operation, "'"};

        description.kwargs.insert_or_assign(std::string{token.substr(2, eq - 2)}, std::string{token.substr(eq + 1)});
      }
    } else
      description.args.emplace_back(token);
  }

  return descriptions;
}

std::vector<std::string> OperationDescription::tokenize(std::string_view description) {
  std::vector<std::string> tokens;
  std::string token;
  bool in_token = false;
  bool in_quotes = false;

  for (size_t i = 0; i < description.size(); i++) {
    char c = description[i];

    if (c == '\\' && i + 1 < description.size() && (description[i + 1] == '"' || description[i + 1] == '\\')) {
      token.push_back(description[++i]);
      in_token = true;
    } else if (c == '"') {
      in_quotes = !in_quotes;
      in_token = true;
    } else if (!in_quotes && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
      if (in_token) {
        tokens.push_back(std::move(token));
        token.clear();
        in_token = false;
      }
    } else {
      token.push_back(c);
      in_token = true;
    }
  }

  if (in_quotes)
    throw LinpipeError{"OperationDescription::tokenize: Closing quote missing in description '", description, "'"};

  if (in_token)
    tokens.push_back(std::move(token));

  return tokens;
}

} // namespace linpipe
