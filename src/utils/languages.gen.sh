#!/bin/sh

# Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set -e

rm -f iso-639-3.tab
wget https://iso639-3.sil.org/sites/iso639-3/files/downloads/iso-639-3.tab
sed '1d' iso-639-3.tab -i

git clone --depth=1 --branch=pages-source https://github.com/UniversalDependencies/docs
git clone --depth=1 https://github.com/UniversalDependencies/docs-automation

mkdir -p flags
awk '
  /^[^ ]/ { name = substr($0, 0, length($0) - 1) }
  /  flag:/ { flag = $2; gsub(/'"'"'/, "", flag) }
  /  iso3:/ { iso3 = $2; gsub(/'"'"'/, "", iso3); print name "\t" flag "\t" iso3 }
' docs-automation/codes_and_flags.yaml | while IFS=$(printf '\t') read name flag iso3; do
  inkscape docs/flags/svg/$flag.svg -o flags/$iso3.png -h 32
  optipng -clobber -o7 -strip all flags/$iso3.png
  grep -q "^$iso3" iso-639-3.tab || echo "$iso3$(printf '\t\t\t\t\t\t')$name" >>iso-639-3.tab
done

sed "s@[0-9][0-9]*> languages@$(cat iso-639-3.tab | wc -l)> languages@" languages.h -i
python3 languages.gen.py

rm -rf iso-639-3.tab docs/ docs-automation/ flags/
