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

git clone --depth=1 --branch=v4.4.0 https://github.com/ufal/unilib unilib_git

mv unilib_git/unilib/*.h unilib_git/unilib/*.cpp unilib_git/AUTHORS unilib_git/CHANGES.md unilib_git/LICENSE unilib_git/README.md unilib
sed 's/namespace unilib/namespace linpipe::unilib/' -i unilib/*.cpp unilib/*.h

rm -rf unilib_git
