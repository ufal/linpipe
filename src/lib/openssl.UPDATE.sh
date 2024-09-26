#!/bin/bash

# This file is part of C++-Builtem <http://github.com/ufal/cpp_builtem/>.
#
# Copyright 2014-2023 Institute of Formal and Applied Linguistics, Faculty
# of Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

[ -n "$BASH_VERSION" ] || { echo This script must be run using bash, terminating. >&2; exit 1; }

set -e

LIB=$(readlink -f $(dirname $0))

git clone --depth=1 --branch=openssl-3.3.2 https://github.com/openssl/openssl /tmp/openssl

(
cd /tmp/openssl

./Configure --release --prefix=/usr/lib no-afalgeng no-asm no-async no-autoload-config no-capieng no-cmp no-cms no-comp no-ct no-deprecated no-dgram no-dso no-dynamic-engine no-engine no-filenames no-gost no-legacy no-module no-multiblock no-ocsp no-padlockeng no-rdrand no-shared no-srp no-srtp no-ssl-trace no-static-engine no-threads

make -j4 >& make.log

sources=($(grep -o '\(crypto\|providers\|ssl\)/[^[:space:]]*[.]c' make.log | sort -u))

while [ -n "${sources[0]}" ]; do
  source=${sources[0]}
  sources=(${sources[@]:1})
  [ -f "$LIB/openssl/$source" ] && continue

  dir_src=$(dirname $source)
  dir_tgt=$LIB/openssl/$dir_src
  mkdir -p $dir_tgt
  additional_sources=$(cat $source | perl -ple '
    if (/^(\s*#\s*include\s*)["<]([^">]*)[">](.*)$/) {
      my $remapped;
      -e "'$dir_src'/$2" and $remapped="'$dir_src'/$2";
      -e "$2" and $remapped="$2";
      -e "include/$2" and $remapped="include/$2";
      -e "providers/common/include/$2" and $remapped="providers/common/include/$2";
      -e "providers/implementations/include/$2" and $remapped="providers/implementations/include/$2";
      $remapped and $_="$1\"lib/openssl/$remapped\"$3";
      $remapped and $remapped !~ /ms\/uplink\.h/ and print STDERR $remapped;
    }
  ' 2>&1 >$dir_tgt/$(basename $source))
  sources=(${sources[@]} $additional_sources)
done

for macro_value in OPENSSL_USE_NODELETE: L_ENDIAN: OPENSSLDIR:"\"/dev/null\"" ENGINESDIR:"\"/dev/null\"" MODULESDIR:"\"/dev/null\""; do
  macro=${macro_value%%:*}
  value=${macro_value#*:}
  for path in $(grep -lw $macro -R $LIB/openssl); do
    sed "1i#ifndef $macro
         1i#define $macro${value:+ $value}
         1i#endif" -i $path
  done
done

(
grep '^#' $LIB/../Makefile.include.sh
cat <<"EOF"

$(call obj,lib/openssl/%): C_FLAGS := $(filter-out $(default_warnings) $(treat_warnings_as_errors),$(C_FLAGS))
$(call obj,lib/openssl/%): C_FLAGS += $(if $(filter win-vs-%,$(PLATFORM)),$(call define_macro,WIN32_LEAN_AND_MEAN) /wd4090 /wd4133)

EOF
grep -o '\(crypto\|providers\|ssl\)/[^[:space:]]*[.]c' make.log | sort -u | sed 's@^@LINPIPE_OBJECTS += lib/openssl/@; s@[.]c$@@'
) >$LIB/openssl/Makefile.include

)

patch openssl/include/openssl/configuration.h <<"EOF"
--- openssl/include/openssl/configuration.h.orig	2024-09-27 00:01:13.132539197 +0200
+++ openssl/include/openssl/configuration.h	2024-09-27 00:02:08.644352676 +0200
@@ -232,11 +232,23 @@
  * The following are cipher-specific, but are part of the public API.
  */
 # if !defined(OPENSSL_SYS_UEFI)
-#  undef BN_LLONG
-/* Only one for the following should be defined */
-#  define SIXTY_FOUR_BIT_LONG
-#  undef SIXTY_FOUR_BIT
-#  undef THIRTY_TWO_BIT
+#  if _WIN32 || _WIN64
+#   if _WIN64
+#    define SIXTY_FOUR_BIT
+#   else
+#    define THIRTY_TWO_BIT
+#    define BN_LLONG
+#   endif
+#  elif __GNUC__
+#   if __x86_64__ || __aarch64__
+#    define SIXTY_FOUR_BIT_LONG
+#   else
+#    define THIRTY_TWO_BIT
+#    define BN_LLONG
+#   endif
+#  else
+#   error Cannot determine platform for OpenSSL.
+#  endif
 # endif
 
 # define RC4_INT unsigned int
EOF

patch openssl/include/crypto/bn_conf.h <<"EOF"
--- bn_conf.h.ori	2023-10-13 13:22:11.359860627 +0200
+++ bn_conf.h	2023-10-13 13:23:19.164699450 +0200
@@ -21,9 +21,6 @@
 
 /* Should we define BN_DIV2W here? */
 
-/* Only one for the following should be defined */
-#define SIXTY_FOUR_BIT_LONG
-#undef SIXTY_FOUR_BIT
-#undef THIRTY_TWO_BIT
+# include "lib/openssl/include/openssl/configuration.h"
 
 #endif
EOF

rm -rf /tmp/openssl
