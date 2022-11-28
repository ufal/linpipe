// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
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
  if (args["model"] == "rule_based") _tokenizer = make_unique<RuleBasedTokenizer>();

  _model_names = _tokenizer->model_names();
  _source = args["source"];
  _target = args["target"];
}

void Tokenize::execute(Corpus& corpus, PipelineState& /*state*/) {
  for (auto& doc : corpus.documents) {
    auto& source = doc->get_layer<layers::Text>(_source);
    auto target = make_unique<layers::Tokens>(_target);

    _tokenizer->tokenize(source.text, target->tokens);

    doc->add_layer(move(target));
  }
}

} // namespace linpipe::operations
