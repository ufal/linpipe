// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>

#include "operations/load.h"
#include "utils/arguments.h"

namespace linpipe::operations {

Load::Load(const string_view description) {
  // Parse arguments
  unordered_map<string_view, string_view> args;
  vector<string_view> kwargs;
  // TODO: make "lif" default once implemented
  // args["format"] = "lif";
  args["format"] = "text";

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
    for (string_view source_path : _source_paths) {
      ifstream input_file;
      input_file.open({source_path.begin(), source_path.end()});
      if (!input_file) {
        throw LinpipeError{"Could not open source path '", source_path, "' for reading"};
      }
      _read_from_handle(corpus, input_file, source_path);
    }
  }
}

void Load::_read_from_handle(Corpus& corpus, istream& input, const string_view source_path) {
  bool documents_to_read = true;
  while (documents_to_read) {
    Document doc;
    documents_to_read = _format->load(doc, input, source_path);
    corpus.documents.push_back(move(doc));
  }
}

} // namespace linpipe::operations
