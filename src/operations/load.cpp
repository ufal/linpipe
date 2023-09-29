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
  _format = Format::create(args["format"]);
  _source_paths = kwargs;
}

void Load::execute(Corpus& corpus, PipelineState& state) {
  if (_source_paths.empty()) {  // default input
    _read_from_handle(corpus, *state.default_input, "");
  }
  else {  // file inputs
    for (string source_path : _source_paths) {
      ifstream input_file;
      input_file.open(string(source_path));
      if (!input_file) {
        throw LinpipeError{"Could not open source path '", source_path, "' for reading"};
      }
      _read_from_handle(corpus, input_file, source_path);
    }
  }
}

void Load::_read_from_handle(Corpus& corpus, istream& input, const string source_path) {
  unique_ptr<Document> doc;
  while ((doc = _format->load(input, source_path)))
    corpus.documents.push_back(std::move(doc));
}

} // namespace linpipe::operations
