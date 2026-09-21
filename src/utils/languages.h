// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"

namespace linpipe {

class Language {
  public:
    string name_;
    string iso639_1;
    vector<string> non_iso639_1_codes;
};

// Raw generated data, produced by generate_languages_cpp.py.
extern const vector<Language> allLanguages;

class Languages {
  public:
    Languages() : languages_(allLanguages) {
      for (size_t i = 0; i < languages_.size(); i++) {
        const Language& language = languages_[i];
        name_to_language[language.name_] = i;

        if (!language.iso639_1.empty())
          iso_639_1_to_language[language.iso639_1] = i;

        for (const string& code : language.non_iso639_1_codes)
          non_iso_639_1_to_language[code] = i;
      }
    }

    Language& language_by_name(const string& name) {
      auto it = name_to_language.find(name);
      if (it == name_to_language.end())
        throw LinpipeError{"Unknown language name: '", name, "'"};
      return languages_[it->second];
    }

    Language& language_by_iso639_1(const string& iso_639_1) {
      auto it = iso_639_1_to_language.find(iso_639_1);
      if (it == iso_639_1_to_language.end())
        throw LinpipeError{"Unknown ISO 639-1 code: '", iso_639_1, "'"};
      return languages_[it->second];
    }

    Language& language_by_non_iso_639_1(const string& iso_639_1) {
      auto it = non_iso_639_1_to_language.find(iso_639_1);
      if (it == non_iso_639_1_to_language.end())
        throw LinpipeError{"Unknown non-ISO-639-1 code: '", iso_639_1, "'"};
      return languages_[it->second];
    }

  private:
    vector<Language> languages_;

    unordered_map<string, int> name_to_language;
    unordered_map<string, int> iso_639_1_to_language;
    unordered_map<string, int> non_iso_639_1_to_language;
};

} // namespace linpipe
