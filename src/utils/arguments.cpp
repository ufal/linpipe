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
  // Operations start with "--" (e.g., " --tag"), named arguments of an
  // operation start with a single "-" (e.g., " -batch_size 32").
  size_t start = 0;

  while (start < description.length()) {
    // At this position, operation name should be found
    size_t op = find_next_operation_(description, start);

    if (op != start) {
      throw LinpipeError{"Arguments::parse_operations: Operation name expected in description at position '", description.substr(start), "'"};
    }

    // Find next operation, skipping the leading " --" of the current one
    size_t next = find_next_operation_(description, op+3);
    descriptions.push_back(description.substr(op, next-op));

    start = next;
  }
}

void Arguments::parse_arguments(std::unordered_map<std::string, std::string>& args, std::vector<std::string>& kwargs, const std::string description) {
  // Everything must be separated by space.
  // Named arguments start with a single "-" and their value is either the
  // next token (-format text), or follows the first "=" (-format=text).
  // Operation names start with "--" and are skipped here.
  // TODO: Add quotes.

  size_t start = 1; // skip leading space
  size_t pos = 0;
  std::string argument = "";
  while (pos != std::string::npos) {
    pos = description.find(' ', start);
    std::string token = description.substr(start, pos-start);

    if (start > 1) { // skip operation name
      if (!argument.empty()) { // value of the preceding argument (may start with '-', e.g. -1)
        args[argument] = token;
        argument = "";
      }
      else if (token.size() > 1 && token[0] == '-' && token[1] != '-') { // argument found
        size_t eq = token.find('=');
        if (eq == std::string::npos) { // value is the next token
          argument = token.substr(1);
        }
        else { // -name=value, split on the first '=' only (values may contain '=')
          if (eq == 1) {
            throw LinpipeError{"Arguments::parse_arguments: Argument name expected before '=' in '", token, "' in description '", description, "'"};
          }
          args[token.substr(1, eq-1)] = token.substr(eq+1);
        }
      }
      else {
        kwargs.push_back(token);
      }
    }

    start = pos+1;
  }
}

void Arguments::parse_format(std::unordered_map<std::string, std::string>& args, const std::string description) {
  /* Parses format key-value arguments.

  Receives:
    description: structured format string description with key-value pairs,
      separated by a ',', key separated from value by a '='.
      For example -format conll-2003 translates as conll with the following
      setting:
      conll(1=name:type,2=:lemmas,2_default=_,3=:chunks,3_default=_,4=:named_entities,4_encoding=bio)

  Returns:
    args: unordered map of key (string) to value (string) pairs.
  */

  // Remove leading format name and brackets (if present)
  std::string format_description = description;
  size_t pos = description.find("(");
  if (pos != std::string::npos) {
    format_description = description.substr(pos+1); // remove format name & opening bracket
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

size_t Arguments::find_next_operation_(const std::string description, size_t offset) {
  // Returns the position of the space preceding the next operation
  // (" --name"), or std::string::npos if there is none.

  while (offset < description.length()) {
    size_t op = description.find(" --", offset);

    if (op == std::string::npos) { // not found
      return std::string::npos;
    }

    if (op + 3 >= description.length() || description[op+3] == ' ') { // "--" without operation name
      throw LinpipeError{"Arguments::find_next_operation_: Operation name expected after '--' in description '", description, "'"};
    }

    if (description[op+3] != '-') { // operation found
      return op;
    }

    offset = op+3; // "---" is not an operation, search further
  }

  return std::string::npos;
}

} // namespace linpipe
