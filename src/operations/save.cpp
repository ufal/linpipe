// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>

#include "operations/save.h"
#include "utils/arguments.h"

namespace linpipe::operations {

Save::Save(const string_view description) {
  // Parse arguments
  unordered_map<string, string> args;
  vector<string> kwargs;
  // TODO: make "lif" default once implemented
  // args["format"] = "lif";
  args["format"] = "text";

  Arguments arguments;
  arguments.parse_arguments(args, kwargs, description);

  // Process parsed arguments
  _format = Format::create(args["format"]);
  _target_paths = kwargs;
}

void Save::execute(Corpus& corpus, PipelineState& state) {
  if (_target_paths.empty()) { // no required outputs
    for (unsigned int i = 0; i < corpus.documents.size(); i++) {
      if (corpus.documents[i].source_path().empty()) {  // was read from cin => print to cout
        _format->save(corpus.documents[i], *state.default_output);
      }
      else {
        // TODO: derive a similar target path by appending some other extension
        // to document's source path and print there
      }
    }
  }
  else { // custom target paths
    if (_target_paths.size() == 1) {  // append everything to one file
      ofstream output_file;
      output_file.open(_target_paths[0]);
      if (!output_file) {
        throw LinpipeError{"Could not open target path '", _target_paths[0], "' for writing"};
      }
      for (unsigned int i = 0; i < corpus.documents.size(); i++) {
        _format->save(corpus.documents[i], output_file);
      }
    }
    else { // 1 target path = 1 document
      if (_target_paths.size() != corpus.documents.size()) {
        throw LinpipeError{"Number of target paths in -save operation is not equal to the number of documents in the corpus'"};
      }
      for (unsigned int i = 0; i < _target_paths.size(); i++) {
        ofstream output_file;
        output_file.open(_target_paths[i]);
        if (!output_file) {
          throw LinpipeError{"Could not open target path '", _target_paths[i], "' for writing"};
        }
        _format->save(corpus.documents[i], output_file);
      }
    }
  }
}

} // namespace linpipe::operations
