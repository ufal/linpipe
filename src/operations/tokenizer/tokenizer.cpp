// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "operations/tokenizer/tokenizer.h"

namespace linpipe::operations {

vector<string> Tokenizer::model_names() {
  return _model_names;
}

string Tokenizer::type() {
  return _type;
}

} // namespace linpipe::operations
