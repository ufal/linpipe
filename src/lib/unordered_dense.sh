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

[ -d unordered_dense_git ] && rm -rf unordered_dense_git/
git clone --depth=1 --branch=v5.3.2 https://github.com/martinus/unordered_dense unordered_dense_git

sed -e '
  /^ *# *include *"stl.h" *$/{
    r unordered_dense_git/include/ankerl/stl.h
    d
  }
  s/\([ (]\)ankerl::/\1linpipe::ankerl::/
' unordered_dense_git/include/ankerl/unordered_dense.h > unordered_dense/unordered_dense.h

rm -rf unordered_dense_git/
