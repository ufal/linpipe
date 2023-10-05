// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "utils/split.h"

namespace linpipe {

size_t split(string_view str, char delimiter, vector<string_view>& parts, size_t max_splits) {
  parts.clear();

  size_t splits = 0, index = 0;
  while (true) {
    size_t pos = str.find(delimiter, index);
    if (pos == string::npos) break;

    parts.push_back(str.substr(index, pos - index));
    index = pos + 1;
    splits++;
    if (max_splits > 0 && splits >= max_splits)
      break;
  }

  if (index < str.size() || index) {
    parts.push_back(str.substr(index));
    splits++;
  }
  return splits;
}

size_t split(string_view str, string_view delimiter, vector<string_view>& parts, size_t max_splits) {
  parts.clear();

  size_t splits = 0, index = 0;
  while (true) {
    size_t pos = str.find(delimiter, index);
    if (pos == string::npos) break;

    parts.push_back(str.substr(index, pos - index));
    index = pos + delimiter.size();
    splits++;
    if (max_splits > 0 && splits >= max_splits)
      break;
  }

  if (index < str.size() || index) {
    parts.push_back(str.substr(index));
    splits++;
  }
  return splits;
}

} // namespace linpipe
