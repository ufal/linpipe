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

git clone --depth=1 --branch=master https://github.com/google/sentencepiece sentencepiece_git
(cd sentencepiece_git && cmake -B build && ln -s ../../build/third_party/flatbuffers-src build/_deps/flatbuffers-src && cmake --build build --target amalgamate -j4)

cp sentencepiece_git/dist/sentencepiece_lite.cc sentencepiece/sentencepiece_lite.cpp
cp sentencepiece_git/dist/sentencepiece_lite.h sentencepiece/sentencepiece_lite.h

perl -e '
  my $in_include = 1;
  while (<>) {
    s/::flatbuffers::/::linpipe::flatbuffers::/g;
    if ($in_include && !/^\s*(#include|\/\/|\/\*|\*|$)/) {
      print "namespace linpipe {\n";
      $in_include = 0;
    } elsif (!$in_include && /^\s*#include/) {
      print "} // namespace linpipe\n";
      $in_include = 1;
    }
    print;
    if (eof) {
      print "\n} // namespace linpipe\n" if !$in_include;
      $in_include = 1;
    }
  }
' -i sentencepiece/sentencepiece_lite.[ch]*

rm -rf sentencepiece_git
