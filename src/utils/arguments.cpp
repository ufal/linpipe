// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "utils/arguments.h"

namespace linpipe {

void Arguments::parse_operations(vector<string>& descriptions, const string description) {
  size_t start = 0;

  while (start < description.length()) {
    // At this position, operation name should be found
    size_t op = _find_next_operation(description, start);

    if (op != start) {
      throw LinpipeError{"Operation name expected in description at position '", description.substr(start), "'"};
    }

    // Find next operation
    size_t next = _find_next_operation(description, op+2);
    descriptions.push_back(description.substr(op, next-op));

    start = next;
  }
}

void Arguments::parse_arguments(unordered_map<string, string>& args, vector<string>& kwargs, const string description) {
  // Everything must be separated by space.
  // TODO: Add values separated by "=" (--format="text") and quotes.

  size_t start = 1; // skip leading space
  size_t pos = 0;
  string argument = "";
  while (pos != string::npos) {
    pos = description.find(' ', start);
    string token = description.substr(start, pos-start);

    if (start > 1) { // skip operation name
      if (token.find("--") == 0) { // argument found
        argument = token.substr(2);
      }
      else {
        if (argument.empty()) {
          kwargs.push_back(token);
        }
        else {
          args[argument] = token;
          argument = "";
        }
      }
    }

    start = pos+1;
  }
}

void Arguments::parse_format(unordered_map<string, string>& args, const string description) {
  /* Parses format key-value arguments.

  Receives:
    description: structured format string description with key-value pairs,
      separated by a ',', key separated from value by '='.
      For example --format conll-2003 translates as conll with the following
      setting:
      1=name:type,2=:lemmas,2_default=_,3=:chunks,3_default=_,4=:named_entities,4_encoding=bio

  Returns:
    args: unordered map of string key to string value pairs.
  */

  vector<string> tokens;
  _string_helper.split(tokens, description, ",");
  for (string token : tokens) {
    vector<string> pair;
    _string_helper.split(pair, token, "=");
    if (pair.size() != 2) {
      throw LinpipeError{"Expected key-value pair separated by '=' in '", token, "' in format description '", description, "'"};
    }
    args[pair[0]] = pair[1];
  }
}

size_t Arguments::_find_next_operation(const string description, size_t offset) {

  while (offset < description.length()) {
    size_t op = description.find(" -", offset);

    if (op == string::npos) { // not found
      return string::npos;
    }

    if (op + 2 == string::npos) { // description too short
      return string::npos;
    }

    if (description[op+2] == ' ') { // invalid description
      return string::npos;
    }

    if (description[op+2] != '-') { // operation found
      return op;
    }

    offset = op+2; // argument found, search further
  }

  return string::npos;
}

} // namespace linpipe
