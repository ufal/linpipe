// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>

#include "operations/load.h"
#include "utils/arguments.h"

namespace linpipe::operations {

Load::Load(const string description) {
  // Parse arguments
  unordered_map<string, string> args;
  vector<string> kwargs;
  args["format"] = "lif";

  Arguments arguments;
  arguments.parse_arguments(args, kwargs, description);

  // Process parsed arguments
  format_ = Format::create(args["format"]);
  source_paths_ = kwargs;
}

void Load::execute(Corpus& corpus, PipelineState& state) {
  if (source_paths_.empty()) {  // default input
    read_from_handle_(corpus, *state.default_input, "");
  }
  else {  // file inputs
    for (string source_path : source_paths_) {
      ifstream input_file;
      input_file.open(string(source_path));
      if (!input_file) {
        throw LinpipeError{"Load::execute: Could not open source path '", source_path, "' for reading"};
      }
      read_from_handle_(corpus, input_file, source_path);
    }
  }
}

void Load::read_from_handle_(Corpus& corpus, istream& input, const string source_path) {
  unique_ptr<Document> doc;
  while ((doc = format_->load(input, source_path)))
    corpus.documents.push_back(std::move(doc));
}

} // namespace linpipe::operations
