// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <fstream>

#include "common.h"

int main(int argc, char* argv[]) {
  auto is_ascii = [](char c) { return (c >= ' ' && c <= '~') || c == '\t' || c == '\r'; };

  bool valid = true;
  for (int i = 1; i < argc; i++) {
    std::ifstream file(argv[i], std::ios::binary);
    if (!file) {
      std::cerr << argv[i] << ": cannot open the file" << '\n';
      valid = false;
      continue;
    }

    std::string line;
    for (size_t number = 1; std::getline(file, line); number++)
      if (!std::ranges::all_of(line, is_ascii)) {
        std::cerr << argv[i] << ':' << number << ": non-ASCII or control character" << '\n';
        valid = false;
      }
  }

  return valid ? 0 : 1;
}
