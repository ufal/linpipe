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

void Arguments::parse_operations(vector<string_view>& operations, const string_view description) {
  size_t start = 0;

  while (start != string_view::npos) {
    // At this position, operation name should be found
    size_t op = _find_next_operation(description, start);

    if (op == string_view::npos || op != start) {
      throw LinpipeError{"Operation name expected in description at position '", description.substr(start), "'"};
    }

    // Find next operation
    size_t next = _find_next_operation(description, op+2);
    if (next == string_view::npos) { // can I subtract from string_view::npos?
      operations.push_back(description.substr(op));
    }
    else {
      operations.push_back(description.substr(op, next-op));
    }

    start = next;
  }
}

void Arguments::parse(unordered_map<string, string>& /*args*/, const string_view /*description*/) {
  // TODO
}

size_t Arguments::_find_next_operation(const string_view description, size_t offset) {

  while (offset != string_view::npos) {
    size_t op = description.rfind(" -", offset);

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

    offset = op; // argument found, search further
  }

  return offset;
}

} // namespace linpipe
