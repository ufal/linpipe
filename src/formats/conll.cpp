// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/conll.h"

namespace linpipe::formats {

Conll::Conll(const string description) : Format("conll") {
  size_t start = 0;
  size_t pos = 0;
  while (pos != string::npos) {
    pos = description.find(":", start);
    _descriptions.push_back(description.substr(start, pos));
    start = pos+1;
  }
}

bool Conll::load(Document& document, istream& input, const string source_path) {
  string line;

  if (!getline(input, line)) {
    return false;
  }

  // TODO

  document.set_source_path(source_path);

  return true;
}

void Conll::save(Document& /*document*/, ostream& /*output*/) {
  // TODO
}

} // namespace linpipe::formats
