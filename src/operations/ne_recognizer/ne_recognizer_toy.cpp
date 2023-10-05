// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "operations/ne_recognizer/ne_recognizer_toy.h"

namespace linpipe::operations {

void NERecognizerToy::recognize(ModelManager* /*model_manager*/, const vector<string>& /*text*/, vector<pair<unsigned, unsigned>>& /*spans*/) {
  /* Toy NE Recognizer class with model. */

  //Model* model = model_manager->load(model_names_[0]);
  // TODO: Use model to recognize named entities.
};

} // namespace linpipe::operations
