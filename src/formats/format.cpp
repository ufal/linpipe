// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "formats/conll.h"
#include "formats/format.h"
#include "formats/lif.h"
#include "formats/text.h"

namespace linpipe {

unique_ptr<Format> Format::create(const string description) {
  if (description == "text") {
    return make_unique<formats::Text>();
  }
  if (description == "lif") {
    return make_unique<formats::Lif>();
  }
  if (description == "conll") {
    return make_unique<formats::Conll>("conll:tokens");
  }
  if (description.find("conll:") == 0) {
    return make_unique<formats::Conll>(description);
  }

  throw LinpipeError{"Invalid value '", description, "' for the --format argument of the -load operation'"};
}

const string& Format::name() {
  return _name;
}

void Format::set_name(const string name) {
  _name = name;
}

void Format::save_corpus_start(ostream& /*output*/) {
}

void Format::save_corpus_end(ostream& /*output*/) {
}

} // namespace linpipe
