// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "models/model.h"
#include "models/ner_toy.h"

namespace linpipe {

const string& Model::name() {
  return _name;
}

unique_ptr<Model> Model::create(string& name, istream& input) {
  if (name == "ner_toy") return make_unique<models::NERToy>(name, input);

  throw LinpipeError{"Cannot load model of uknown name '", name, "'"};
}

} // namespace linpipe
