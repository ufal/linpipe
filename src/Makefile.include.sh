[ -n "$1" -a -d "$1" ] || { echo Usage: sh $0 directory >&2; exit 1; }

cat >"$1"/Makefile.include <<EOF
# This file is part of LinPipe <http://github.com/ufal/linpipe/>.
#
# Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

EOF

add_newline=""
for f in $(ls "$1"/*.cpp | grep -v _test.cpp$); do
  [ -f "$f" ] || continue
  add_newline=1
  echo LINPIPE_OBJECTS += "${f%.cpp}"
done >>"$1"/Makefile.include

for f in "$1"/*_test.cpp; do
  [ -f "$f" ] || continue
  [ -n "$add_newline" ] && echo
  add_newline=
  echo LINPIPE_TESTS += "${f%.cpp}"
done >>"$1"/Makefile.include
