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

git clone --depth=1 --branch=12.2.0 https://github.com/fmtlib/fmt fmt_git

for header in base.h core.h format.h format-inl.h ranges.h; do
  cp fmt_git/include/fmt/$header fmt/$header
done
sed '
  1i #define FMT_BEGIN_NAMESPACE namespace linpipe::fmt {
  1i #define FMT_END_NAMESPACE } // namespace linpipe::fmt
  s@ ::fmt@ ::linpipe::fmt@
' -i fmt/base.h
sed 's@<fmt::@<linpipe::fmt::@' -i fmt/format.h

sed 's@#include *"fmt/@#include "@' fmt_git/src/format.cc >fmt/format.cpp

rm -rf fmt_git/
