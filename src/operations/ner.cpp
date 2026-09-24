// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "layers/token_layer.h"
#include "layers/spans.h"
#include "operations/ner.h"
#include "operations/ne_recognizer/ne_recognizer_toy.h"
#include "utils/arguments.h"

namespace linpipe::operations {

NER::NER(const std::string description) {
  // Parse arguments
  std::unordered_map<std::string, std::string> args;
  std::vector<std::string> kwargs;
  args["model"] = "ner_toy";
  args["source"] = "";
  args["target"] = "";

  Arguments arguments;
  arguments.parse_arguments(args, kwargs, description);

  // Process parsed arguments
  if (args["model"] == "ner_toy") ne_recognizer_ = std::make_unique<NERecognizerToy>(std::vector<std::string>{args["model"]});

  model_names_ = ne_recognizer_->model_names();
  source_ = args["source"];
  target_ = args["target"];
}

void NER::apply(Corpus& corpus, PipelineState& state) {
  for (auto& doc : corpus.documents) {
    auto& source = doc->get_layer<layers::TokenLayer>(source_);
    auto target = std::make_unique<layers::Spans>(target_);

    ne_recognizer_->recognize(state.model_manager, source.tokens, target->spans);

    doc->add_layer(std::move(target));
  }
}

} // namespace linpipe::operations
