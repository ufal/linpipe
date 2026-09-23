// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "operations/tokenize.h"
#include "operations/tokenizer/rule_based_tokenizer.h"
#include "utils/arguments.h"
#include "layers/plain_text.h"
#include "layers/token_layer.h"

namespace linpipe::operations {

Tokenize::Tokenize(const std::string description) {
  // Parse arguments
  std::unordered_map<std::string, std::string> args;
  std::vector<std::string> kwargs;
  args["model"] = "rule_based";
  args["source"] = "";
  args["target"] = "";

  Arguments arguments;
  arguments.parse_arguments(args, kwargs, description);

  // Process parsed arguments
  if (args["model"] == "rule_based") tokenizer_ = std::make_unique<RuleBasedTokenizer>(std::vector<std::string>{args["model"]});

  model_names_ = tokenizer_->model_names();
  source_ = args["source"];
  target_ = args["target"];
}

void Tokenize::execute(Corpus& corpus, PipelineState& state) {
  for (auto& doc : corpus.documents) {
    auto& source = doc->get_layer<layers::PlainText>(source_);
    auto target = std::make_unique<layers::TokenLayer>(target_);

    tokenizer_->tokenize(state.model_manager, source.text, target->tokens);

    doc->add_layer(std::move(target));
  }
}

} // namespace linpipe::operations
