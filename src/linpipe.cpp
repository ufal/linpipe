// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "version/version.h"

using namespace linpipe;

std::string version_and_copyright() {
  return fmt::format(
      "LinPipe version {}.\n"
      "Copyright (C) 2022-2026 Institute of Formal and Applied Linguistics, Faculty\n"
      "of Mathematics and Physics, Charles University in Prague, Czech Republic.\n",
      Version::current().as_string());
}

std::string usage() {
  return (
      "Usage: linpipe [options] <pipeline_description>\n"
      "Options:\n"
      "  --usage         Show this help message and exit\n"
      "  --version       Show version information and exit\n"
      "For documentation including the pipeline description syntax and examples,\n"
      "please visit the LinPipe homepage <https://linpipe.org/>.\n");
}

int main(int argc, char* argv[]) {
  std::iostream::sync_with_stdio(false);

  std::vector<std::string_view> args(argv + 1, argv + argc);

  if (args.empty() || args[0] == "--usage") {
    std::cout << version_and_copyright();
    std::cout << '\n';
    std::cout << usage();
    return 0;
  }

  if (args[0] == "--version") {
    std::cout << version_and_copyright();
    return 0;
  }

  try {
    LOG(INFO, "LinPipe " << Version::current().as_string() << " is up and running");
  } catch (const LinpipeError& error) {
    LOG(FATAL, "An unhandled exception has occurred, terminating: " << error.what());
    return 1;
  }

  return 0;
}
