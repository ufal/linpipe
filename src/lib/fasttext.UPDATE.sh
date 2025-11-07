#!/bin/sh

# This file is part of C++-Builtem <http://github.com/ufal/cpp_builtem/>.
#
# Copyright 2014-2023 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set -e

mkdir -p fasttext
rm -f fasttext/*.h fasttext/*.cpp

git clone --depth=1 --branch=v0.9.2 https://github.com/facebookresearch/fastText fasttext_git

for f in fasttext_git/src/*; do
  case "$f" in
    */main.cc) continue;;
    *.cc) target=$(basename $f .cc).cpp;;
    *) target=$(basename $f);;
  esac
  sed 's/namespace fasttext/namespace linpipe::fasttext/;
       s/fasttext::/linpipe::fasttext::/' $f >fasttext/$target
done

patch fasttext/args.h <<EOF
--- a/src/lib/fasttext/args.h
+++ b/src/lib/fasttext/args.h
@@ -8,6 +8,7 @@

 #pragma once

+#include <cstdint>
 #include <istream>
 #include <ostream>
 #include <string>
EOF

rm -rf fasttext_git
