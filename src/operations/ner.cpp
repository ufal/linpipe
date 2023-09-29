// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/tokens.h"
#include "layers/spans.h"
#include "operations/ner.h"
#include "operations/ne_recognizer/ne_recognizer_toy.h"
#include "utils/arguments.h"

namespace linpipe::operations {

NER::NER(const string description) {
  // Parse arguments
  unordered_map<string, string> args;
  vector<string> kwargs;
  args["model"] = "ner_toy";
  args["source"] = "";
  args["target"] = "";

  Arguments arguments;
  arguments.parse_arguments(args, kwargs, description);

  // Process parsed arguments
  if (args["model"] == "ner_toy") _ne_recognizer = make_unique<NERecognizerToy>(vector<string>{args["model"]});

  _model_names = _ne_recognizer->model_names();
  _source = args["source"];
  _target = args["target"];
}

void NER::execute(Corpus& corpus, PipelineState& state) {
  for (auto& doc : corpus.documents) {
    auto& source = doc->get_layer<layers::Tokens>(_source);
    auto target = make_unique<layers::Spans>(_target);

    _ne_recognizer->recognize(state.model_manager, source.tokens, target->spans);

    doc->add_layer(std::move(target));
  }
}

} // namespace linpipe::operations
