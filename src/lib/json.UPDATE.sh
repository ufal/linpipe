#!/bin/sh

# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2014-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set -e

[ -d json_git ] && rm -rf json_git/
git clone --depth=1 --branch=v3.12.0 https://github.com/nlohmann/json json_git

for f in json.hpp json_fwd.hpp; do
  sed '
    /^[ ]*\/\//!s/\(^[ ]*namespace \|[ <(]::\|[ <(]\)\(nlohmann[ :]\)/\1linpipe::\2/
  ' json_git/single_include/nlohmann/$f >json/${f%hpp}h
done

rm -rf json_git/
