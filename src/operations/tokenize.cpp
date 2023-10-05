// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "operations/tokenize.h"
#include "operations/tokenizer/rule_based_tokenizer.h"
#include "utils/arguments.h"
#include "layers/text.h"
#include "layers/tokens.h"

namespace linpipe::operations {

Tokenize::Tokenize(const string description) {
  // Parse arguments
  unordered_map<string, string> args;
  vector<string> kwargs;
  args["model"] = "rule_based";
  args["source"] = "";
  args["target"] = "";

  Arguments arguments;
  arguments.parse_arguments(args, kwargs, description);

  // Process parsed arguments
  if (args["model"] == "rule_based") tokenizer_ = make_unique<RuleBasedTokenizer>(vector<string>{args["model"]});

  model_names_ = tokenizer_->model_names();
  source_ = args["source"];
  target_ = args["target"];
}

void Tokenize::execute(Corpus& corpus, PipelineState& state) {
  for (auto& doc : corpus.documents) {
    auto& source = doc->get_layer<layers::Text>(source_);
    auto target = make_unique<layers::Tokens>(target_);

    tokenizer_->tokenize(state.model_manager, source.text, target->tokens);

    doc->add_layer(std::move(target));
  }
}

} // namespace linpipe::operations
