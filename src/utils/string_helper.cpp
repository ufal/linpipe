// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "string_helper.h"

namespace linpipe {

void StringHelper::split(vector<string>& tokens, const string& str, const string& delim) {
  size_t start = 0;
  size_t pos = 0;
  while (pos != string::npos && start < str.length()) {
    pos = str.find(delim, start);
    tokens.push_back(str.substr(start, pos-start));
    start = pos+1;
  }
}

bool StringHelper::is_number(const string& str) {
  string::const_iterator it = str.begin();
  while (it != str.end() && isdigit(*it))
    it++;
  return !str.empty() && it == str.end();
}

} // namespace linpipe
