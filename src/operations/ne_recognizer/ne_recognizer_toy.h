// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "operations/ne_recognizer/ne_recognizer.h"

namespace linpipe::operations {

class NERecognizerToy : public NERecognizer {
 /* Toy NE Recognizer class with model. */

 public:
  NERecognizerToy(vector<string> model_names) : NERecognizer("ner_toy", model_names) {};
  void recognize(ModelManager* model_manager, const vector<string>& tokens, vector<pair<unsigned, unsigned>>& spans) override;
};

} // namespace linpipe::operations
