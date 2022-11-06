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

Save::Save(const string description) {
  // Parse arguments
  unordered_map<string, string> args;
  vector<string> kwargs;
  args["format"] = "lif";

  Arguments arguments;
  arguments.parse_arguments(args, kwargs, description);

  // Process parsed arguments
  _format = Format::create(args["format"]);
  _target_paths = kwargs;
}

void Save::execute(Corpus& corpus, PipelineState& state) {
  vector<string> target_paths = _target_paths;

  // If user requested custom target paths for documents in format kwargs,
  // check that the number of target paths matches the number of documents.
  if (target_paths.size() && target_paths.size() != corpus.documents.size()) {
    throw LinpipeError{"The number of target paths (", to_string(target_paths.size()), ") != number of documents in the corpus (", to_string(corpus.documents.size()), ")"};
  }

  // If no custom outputs were required in format kwargs, try to figure out the
  // output from the documents' input source_paths.
  if (target_paths.empty()) {
    for (size_t i = 0; i < corpus.documents.size(); i++) {
      if (corpus.documents[i].source_path().empty()) {
        target_paths.push_back(corpus.documents[i].source_path());
      }
      else {
        // TODO: decide on the exact default output extension
        // TODO: the addition ".out" should go BEFORE the actual extension
        target_paths.push_back(corpus.documents[i].source_path() + ".out");
      }
    }
  }

  // Write the documents to their respective target paths.
  ostream* os = state.default_output;
  ofstream os_file;
  for (size_t i = 0; i < corpus.documents.size(); i++) {
    if (i == 0 or target_paths[i] != target_paths[i-1]) {

      // finish writing into previous handle
      if (i > 0) {
        if (target_paths[i] != target_paths[i-1]) {
          _format->save_corpus_end(*os);
        }
        if (!target_paths[i-1].empty()) { // close previous if not cout
          dynamic_cast<ofstream*>(os)->close();
        }
      }

      // open new handle if not cin
      if (!target_paths[i].empty()) {
        os_file.open(target_paths[i]);
        os = &os_file;
        if (!os) {
          throw LinpipeError{"Could not open target path '", target_paths[i], "'"};
        }
      }

      // start writing to new handle
      _format->save_corpus_start(*os);
    }

    // write the document contents
    _format->save(corpus.documents[i], *os);

    // finish writing to the last handle
    if (i == corpus.documents.size() - 1) {
      _format->save_corpus_end(*os);
      if (!target_paths[i].empty()) { // close if not cout
        dynamic_cast<ofstream*>(os)->close();
      }
    }
  }
}

} // namespace linpipe::operations
