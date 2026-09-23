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

[ -d github_doctest ] && rm -rf github_doctest/
git clone --depth=1 --branch=v2.5.3 https://github.com/doctest/doctest github_doctest

sed '
 1i#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
' github_doctest/doctest/doctest.h >doctest.h

rm -rf github_doctest
