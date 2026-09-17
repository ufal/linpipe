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

[ -d cpp-httplib ] && rm -rf cpp-httplib/
git clone --depth=1 --branch=v0.56.0 https://github.com/yhirose/cpp-httplib cpp-httplib

(cd cpp-httplib && python3 split.py -e cpp)

for e in cpp h; do
  sed "s/namespace httplib/namespace linpipe::httplib/" cpp-httplib/out/httplib.$e >httplib.$e
done

sed '
  /^#define CPPHTTPLIB_HTTPLIB_H$/a #define CPPHTTPLIB_MBEDTLS_SUPPORT 1
  /^#define CPPHTTPLIB_HTTPLIB_H$/a #if defined(__APPLE__)
  /^#define CPPHTTPLIB_HTTPLIB_H$/a #define CPPHTTPLIB_USE_CERTS_FROM_MACOSX_KEYCHAIN 1
  /^#define CPPHTTPLIB_HTTPLIB_H$/a #endif

  s@^#include <\(mbedtls\|psa\)\(/[^>]*\)>@#include "lib/mbedtls/include/\1\2"@;
' -i httplib.h

sed 's@#include "httplib.h"@#include "lib/httplib.h"@' -i httplib.cpp

rm -rf cpp-httplib
