// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "arguments.h"
#include "common.h"

namespace linpipe {

void Arguments::parse_operations(vector<string_view>& descriptions, const string_view description) {
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

void Arguments::parse_arguments(unordered_map<string_view, string_view>& args, vector<string_view>& kwargs, const string_view description) {
  // Everything must be separated by space.
  // TODO: Add values separated by "=" (--format="text") and quotes.

  size_t start = 1; // skip leading space
  size_t pos = 0;
  string argument = "";
  while (pos != string_view::npos) {
    pos = description.find(' ', start);
    string_view token = description.substr(start, pos);

    if (start > 1) { // skip operation name
      if (token.find("--") == 0) { // argument found
        argument = token;
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

size_t Arguments::_find_next_operation(const string_view description, size_t offset) {

  while (offset < description.length()) {
    size_t op = description.find(" -", offset);

    if (op == string_view::npos) { // not found
      return string_view::npos;
    }

    if (op + 2 == string_view::npos) { // description too short
      return string_view::npos;
    }

    if (description[op+2] == ' ') { // invalid description
      return string_view::npos;
    }

    if (description[op+2] != '-') { // operation found
      return op;
    }

    offset = op+2; // argument found, search further
  }

  return string_view::npos;
}

} // namespace linpipe
