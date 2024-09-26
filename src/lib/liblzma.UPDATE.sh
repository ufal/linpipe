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

rm -rf liblzma/

git clone --depth=1 --branch=v5.6.2 https://github.com/tukaani-project/xz liblzma_git

cat >liblzma_git/src/common/config.h <<"EOF"
#define HAVE_CHECK_CRC32 1
#define HAVE_CHECK_CRC64 1
#define HAVE_CHECK_SHA256 1
#define HAVE_DECODERS 1
#define HAVE_DECODER_DELTA 1
#define HAVE_DECODER_LZMA1 1
#define HAVE_DECODER_LZMA2 1
#define HAVE_ENCODERS 1
#define HAVE_ENCODER_DELTA 1
#define HAVE_ENCODER_LZMA1 1
#define HAVE_ENCODER_LZMA2 1
#define HAVE_INTTYPES_H 1
#define HAVE_LIMITS_H 1
#define HAVE_MF_BT2 1
#define HAVE_MF_BT3 1
#define HAVE_MF_BT4 1
#define HAVE_MF_HC3 1
#define HAVE_MF_HC4 1
#define HAVE_STDBOOL_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_VISIBILITY 0
EOF

# Copy the sources and their dependences
sources=(
  liblzma_git/src/liblzma/api/lzma.h
  liblzma_git/src/liblzma/check/{check,crc{32,64}_{fast,table},sha256}.c
  liblzma_git/src/liblzma/common/{block*,common*,easy*,file_info,filter*,index*,stream*_{common,decoder,encoder},string_conversion,vli*}.c
  liblzma_git/src/liblzma/delta/delta*.c
  liblzma_git/src/liblzma/lz/lz*.c
  liblzma_git/src/liblzma/lzma/{fastpos_table,lzma*}.c
  liblzma_git/src/liblzma/rangecoder/price_table.c
)

while [ -n "${sources[0]}" ]; do
  source=${sources[0]}
  sources=(${sources[@]:1})

  dir_src=$(dirname $source)
  target=${source#liblzma_git/src/}
  target=${target#common/}
  target=${target#liblzma/}
  target=liblzma/${target}
  [ -f "$target" ] && continue

  mkdir -p $(dirname $target)
  additional_sources=$(cat $source | perl -ple '
    if (/^(\s*#\s*include\s*)["<]([^">]*)[">](.*)$/) {
      -e "liblzma_git/src/common/$2" and $_="$1\"lib/liblzma/$2\"$3" and print STDERR "liblzma_git/src/common/$2";
      foreach my $dir ("api", "check", "common", "delta", "lz", "lzma", "rangecoder", "simple") {
        -e "liblzma_git/src/liblzma/$dir/$2" and $_="$1\"lib/liblzma/$dir/$2\"$3" and print STDERR "liblzma_git/src/liblzma/$dir/$2";
      }
    }
  ' 2>&1 >$target)
  sources=(${sources[@]} $additional_sources)
done
rm liblzma/check/crc{32,64}_table_be.h

# Define required macros
for macro_value in HAVE_CONFIG_H: LZMA_API_STATIC:; do
  macro=${macro_value%%:*}
  value=${macro_value#*:}
  for path in $(grep -lw $macro -R liblzma); do
    sed "1i#ifndef $macro
         1i#define $macro${value:+ $value}
         1i#endif" -i $path
  done
done

# Create the Makefile.include
(
grep '^#' ../Makefile.include.sh
cat <<"EOF"

$(call obj,lib/liblzma/%): C_FLAGS := $(filter-out $(default_warnings) $(treat_warnings_as_errors),$(C_FLAGS))

EOF
find liblzma -name "*.c" | sort -u | sed 's@^@LINPIPE_OBJECTS += lib/@; s@[.]c$@@'
) >liblzma/Makefile.include

rm -rf liblzma_git
