// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "utils/arguments.h"
#include "utils/split.h"

namespace linpipe {

void Arguments::parse_operations(std::vector<std::string>& descriptions, const std::string description) {
  // Operations start with "--" (e.g., "--tag"), named arguments of an
  // operation start with a single "-" (e.g., "-batch_size 32").
  std::vector<std::string> tokens;
  tokenize(tokens, description);

  std::vector<std::string> operation;
  for (const std::string& token : tokens) {
    if (token == "--") {
      throw LinpipeError{"Arguments::parse_operations: Operation name expected after '--' in description '", description, "'"};
    }

    if (is_operation_(token)) {
      if (!operation.empty()) {
        descriptions.push_back(" " + join(operation));
        operation.clear();
      }
    } else if (operation.empty()) {
      throw LinpipeError{"Arguments::parse_operations: Operation name expected at '", token, "' in description '", description, "'"};
    }

    operation.push_back(token);
  }

  if (!operation.empty()) {
    descriptions.push_back(" " + join(operation));
  }
}

void Arguments::parse_arguments(std::unordered_map<std::string, std::string>& args, std::vector<std::string>& kwargs, const std::string description) {
  // Named arguments start with a single "-" and their value is either the
  // next token (-format text), or follows the first "=" (-format=text).
  // Any other token is a positional argument (kwarg).
  std::vector<std::string> tokens;
  tokenize(tokens, description);

  if (tokens.empty() || !is_operation_(tokens[0])) {
    throw LinpipeError{"Arguments::parse_arguments: Operation name expected at the beginning of description '", description, "'"};
  }

  std::string argument = "";
  for (size_t i = 1; i < tokens.size(); i++) { // skip operation name
    const std::string& token = tokens[i];

    if (!argument.empty()) { // value of the preceding argument (may start with '-', e.g. -1)
      args[argument] = token;
      argument = "";
    } else if (token.size() > 1 && token[0] == '-' && token[1] != '-') { // argument found
      size_t eq = token.find('=');
      if (eq == std::string::npos) { // value is the next token
        argument = token.substr(1);
      } else { // -name=value, split on the first '=' only (values may contain '=')
        if (eq == 1) {
          throw LinpipeError{"Arguments::parse_arguments: Argument name expected before '=' in '", token, "' in description '", description, "'"};
        }
        args[token.substr(1, eq - 1)] = token.substr(eq + 1);
      }
    } else {
      kwargs.push_back(token);
    }
  }

  if (!argument.empty()) {
    throw LinpipeError{"Arguments::parse_arguments: Value expected for argument '-", argument, "' in description '", description, "'"};
  }
}

void Arguments::parse_format(std::unordered_map<std::string, std::string>& args, const std::string description) {
  /* Parses format key-value arguments.

  Receives:
    description: structured format string description with key-value pairs,
      separated by a ',', key separated from value by a '='.

  Returns:
    args: unordered map of key (string) to value (string) pairs.
  */

  // Remove leading format name and brackets (if present)
  std::string format_description = description;
  size_t pos = description.find("(");
  if (pos != std::string::npos) {
    format_description = description.substr(pos + 1); // remove format name & opening bracket
    if (format_description.empty()) {
      throw LinpipeError{"Arguments::parse_format: Closing bracket missing in format description '", description, "'"};
    }
    format_description.pop_back();  // remove closing bracket
  }

  std::vector<std::string_view> tokens;
  split(format_description, ',', tokens);
  for (auto& token : tokens) {
    std::vector<std::string_view> pair;
    if (split(token, '=', pair, 1) != 2)
      throw LinpipeError{"Arguments::parse_format: Expected key-value pair separated by '=' in '", token, "' in format description '", description, "'"};
    args.emplace(pair[0], pair[1]);
  }
}

void Arguments::tokenize(std::vector<std::string>& tokens, const std::string& description) {
  std::string token;
  bool in_token = false;   // needed to keep empty quoted tokens ("")
  bool in_quotes = false;

  for (size_t i = 0; i < description.length(); i++) {
    char c = description[i];

    if (c == '\\' && i + 1 < description.length() && (description[i + 1] == '"' || description[i + 1] == '\\')) {
      token.push_back(description[++i]); // escaped quote or backslash
      in_token = true;
    } else if (c == '"') {
      in_quotes = !in_quotes;
      in_token = true;
    } else if (!in_quotes && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
      if (in_token) {
        tokens.push_back(token);
        token.clear();
        in_token = false;
      }
    } else {
      token.push_back(c);
      in_token = true;
    }
  }

  if (in_quotes) {
    throw LinpipeError{"Arguments::tokenize: Closing quote missing in description '", description, "'"};
  }

  if (in_token) {
    tokens.push_back(token);
  }
}

std::string Arguments::join(const std::vector<std::string>& tokens) {
  std::string joined;

  for (size_t i = 0; i < tokens.size(); i++) {
    const std::string& token = tokens[i];

    if (i) {
      joined.push_back(' ');
    }

    if (!token.empty() && token.find_first_of(" \t\n\r\"\\") == std::string::npos) {
      joined.append(token);
    } else { // quote and escape
      joined.push_back('"');
      for (char c : token) {
        if (c == '"' || c == '\\') {
          joined.push_back('\\');
        }
        joined.push_back(c);
      }
      joined.push_back('"');
    }
  }

  return joined;
}

bool Arguments::is_operation_(const std::string& token) {
  // Operation is "--" followed by a name; "---..." is not an operation.
  return token.size() > 2 && token[0] == '-' && token[1] == '-' && token[2] != '-';
}

} // namespace linpipe
