// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace linpipe {

class Language {
 public:
  const char* name;
  const char* iso639_1;
  const char* iso639_2b;
  const char* iso639_3;
  const char* flag;

  const char* canonical_iso() const { return iso639_1 ? iso639_1 : iso639_3; }
};

class Languages {
 public:
  static const std::array<Language, 7937> languages;

  static const Language* language_by_code(std::string_view code);
  static const Language* language_by_name(std::string_view name);
  static const Language* language_by_code_or_name(std::string_view code_or_name);

  static std::string_view canonicalize_code_or_name(std::string_view code_or_name);
};

} // namespace linpipe
