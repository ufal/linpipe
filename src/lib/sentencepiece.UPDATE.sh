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

rm -rf sentencepiece/

git clone --depth=1 --branch=v0.1.99 https://github.com/google/sentencepiece sentencepiece_git
mkdir sentencepiece_git/build
(cd sentencepiece_git/build && cmake ..)

# Copy the sources and their dependences
sources=(
  sentencepiece_git/src/bpe_model.cc
  sentencepiece_git/src/char_model.cc
  sentencepiece_git/src/error.cc
  sentencepiece_git/src/filesystem.cc
  sentencepiece_git/src/model_factory.cc
  sentencepiece_git/src/model_interface.cc
  sentencepiece_git/src/normalizer.cc
  sentencepiece_git/src/sentencepiece_processor.cc
  sentencepiece_git/src/unigram_model.cc
  sentencepiece_git/src/util.cc
  sentencepiece_git/src/word_model.cc
  sentencepiece_git/src/builtin_pb/*.cc
  sentencepiece_git/third_party/protobuf-lite/*.cc
)

while [ -n "${sources[0]}" ]; do
  source=${sources[0]}
  sources=(${sources[@]:1})

  dir_src=$(dirname $source)
  target=${source#sentencepiece_git/}
  target=${target#src/}
  target=${target#build/}
  target=sentencepiece/${target/\.cc/.cpp}
  [ -f "$target" ] && continue

  mkdir -p $(dirname $target)
  additional_sources=$(cat $source | perl -ple '
    if (/^(\s*#\s*include\s*)["<]([^">]*)[">](.*)$/) {
      -e "sentencepiece_git/$2" and $_="$1\"lib/sentencepiece/$2\"$3" and print STDERR "sentencepiece_git/$2";
      -e "sentencepiece_git/src/$2" and $_="$1\"lib/sentencepiece/$2\"$3" and print STDERR "sentencepiece_git/src/$2";
      -e "sentencepiece_git/src/builtin_pb/$2" and $_="$1\"lib/sentencepiece/builtin_pb/$2\"$3" and print STDERR "sentencepiece_git/src/builtin_pb/$2";
      -e "sentencepiece_git/build/$2" and $_="$1\"lib/sentencepiece/$2\"$3" and print STDERR "sentencepiece_git/build/$2";
      -e "sentencepiece_git/third_party/$2" and $_="$1\"lib/sentencepiece/third_party/$2\"$3" and print STDERR "sentencepiece_git/third_party/$2";
      -e "sentencepiece_git/third_party/protobuf-lite/$2" and $_="$1\"lib/sentencepiece/third_party/protobuf-lite/$2\"$3" and print STDERR "sentencepiece_git/third_party/protobuf-lite/$2";
    }
    s/namespace sentencepiece /namespace linpipe::sentencepiece /g;
    s/namespace (absl|Darts|google) /namespace linpipe::sentencepiece::$1 /g;
    s/(?<!linpipe)(::)sentencepiece::/$1linpipe::sentencepiece::/g;
    s/(?<!sentencepiece)(::|\s)(absl|Darts|google)::/$1linpipe::sentencepiece::$2::/g;
  ' 2>&1 >$target)
  sources=(${sources[@]} $additional_sources)
done

# Define required macros
for macro_value in HAVE_PTHREAD:; do
  macro=${macro_value%%:*}
  value=${macro_value#*:}
  for path in $(grep -lw $macro -R sentencepiece); do
    sed "1i#ifndef $macro
         1i#define $macro${value:+ $value}
         1i#endif" -i $path
  done
done

# Remove std::iterator usage
sed '
  / *: public std::iterator<std::output_iterator_tag, T> {/ {
    s/: public std::iterator<std::output_iterator_tag, T>//;
    a  using iterator_category = std::output_iterator_tag;
    a  using value_type = T;
    a  using difference_type = ptrdiff_t;
    a  using pointer = T*;
    a  using reference = T&;
  }
' -i sentencepiece/third_party/protobuf-lite/google/protobuf/repeated_field.h

# Remove unnecessary flag handling
rm -rf sentencepiece/init.h sentencepiece/third_party/absl/flags

# Changes to several files to use our logging system
patch sentencepiece/common.h <<"EOF"
--- common.h.ori	2023-10-18 11:01:04.451560086 +0200
+++ common.h	2023-10-18 11:01:28.731720889 +0200
@@ -25,6 +25,7 @@
 #include <utility>
 #include <vector>
 
+#include "../../common.h"
 #include "lib/sentencepiece/config.h"
 #include "lib/sentencepiece/third_party/absl/strings/string_view.h"
 
@@ -138,23 +139,8 @@
 }  // namespace logging
 }  // namespace sentencepiece
 
-#define LOG(severity)                                                        \
-  (::linpipe::sentencepiece::logging::GetMinLogLevel() >                              \
-   ::linpipe::sentencepiece::logging::LOG_##severity)                                 \
-      ? 0                                                                    \
-      : ::linpipe::sentencepiece::error::Die(                                         \
-            ::linpipe::sentencepiece::logging::LOG_##severity >=                      \
-            ::linpipe::sentencepiece::logging::LOG_FATAL) &                           \
-            std::cerr << ::linpipe::sentencepiece::logging::BaseName(__FILE__) << "(" \
-                      << __LINE__ << ") "                                    \
-                      << "LOG(" << #severity << ") "
-
-#define CHECK(condition)                                                      \
-  (condition) ? 0                                                             \
-              : ::linpipe::sentencepiece::error::Die(true) &                           \
-                    std::cerr << ::linpipe::sentencepiece::logging::BaseName(__FILE__) \
-                              << "(" << __LINE__ << ") [" << #condition       \
-                              << "] "
+#define CHECK(condition) do { if (!(condition)) LOG(FATAL, "Check failed: " << #condition); } while(false)
+#define CHECK_MESSAGE(condition, message) do { if (!(condition)) LOG(FATAL, message << ": " << #condition); } while(false)
 
 #define CHECK_STREQ(a, b) CHECK_EQ(std::string(a), std::string(b))
 #define CHECK_EQ(a, b) CHECK((a) == (b))
EOF

patch sentencepiece/error.cpp <<"EOF"
--- error.cpp.ori	2023-10-18 11:29:24.136058970 +0200
+++ error.cpp	2023-10-18 11:28:15.503569919 +0200
@@ -15,45 +15,9 @@
 #include <cstring>
 
 #include "lib/sentencepiece/common.h"
-#include "lib/sentencepiece/init.h"
 #include "lib/sentencepiece/sentencepiece_processor.h"
 
-#ifdef _USE_EXTERNAL_ABSL
-// Naive workaround to define minloglevel on external absl package.
-// We want to define them in other cc file.
-#include "lib/sentencepiece/third_party/absl/flags/flag.h"
-#include "lib/sentencepiece/third_party/absl/flags/parse.h"
-ABSL_FLAG(int32, minloglevel, 0,
-          "Messages logged at a lower level than this don't actually.");
-#endif
-
 namespace linpipe::sentencepiece {
-namespace error {
-int gTestCounter = 0;
-
-void Abort() {
-  if (GetTestCounter() == 1) {
-    SetTestCounter(2);
-  } else {
-    std::cerr << "Program terminated with an unrecoverable error." << std::endl;
-    ShutdownLibrary();
-    exit(-1);
-  }
-}
-
-void Exit(int code) {
-  if (GetTestCounter() == 1) {
-    SetTestCounter(2);
-  } else {
-    ShutdownLibrary();
-    exit(code);
-  }
-}
-
-void SetTestCounter(int c) { gTestCounter = c; }
-bool GetTestCounter() { return gTestCounter; }
-}  // namespace error
-
 namespace util {
 
 Status::Status() {}
EOF

perl -pge '
  s/LOG\(WARNING\)/LOG(WARN)/g;
  s/(LOG\([A-Z]*)\) << ([^;]*)/$1, $2)/g;
  s/CHECK\(([^;<]*)\) << ([^;]*)/CHECK_MESSAGE($1, $2)/g;
' -i sentencepiece/*.[hc]*

patch sentencepiece/third_party/protobuf-lite/common.cpp <<"EOF"
--- common.cpp.ori	2023-10-18 11:57:38.589638928 +0200
+++ common.cpp	2023-10-18 12:58:32.368474621 +0200
@@ -56,6 +56,7 @@
 #include <android/log.h>
 #endif

+#include "common.h"
 #include "lib/sentencepiece/third_party/protobuf-lite/google/protobuf/stubs/callback.h"
 #include "lib/sentencepiece/third_party/protobuf-lite/google/protobuf/stubs/logging.h"
 #include "lib/sentencepiece/third_party/protobuf-lite/google/protobuf/stubs/once.h"
@@ -184,7 +185,18 @@
   // Nothing.
 }

-static LogHandler* log_handler_ = &DefaultLogHandler;
+void LinpipeLogHandler(LogLevel level, const char* filename, int line, const std::string& message) {
+  if (level == LOGLEVEL_INFO && linpipe::logging_level <= linpipe::LOGGING_INFO)
+    linpipe::logging_start(linpipe::LOGGING_INFO, filename, line) << "[protobuf] " << message << std::endl;
+  if (level == LOGLEVEL_WARNING && linpipe::logging_level <= linpipe::LOGGING_WARN)
+    linpipe::logging_start(linpipe::LOGGING_WARN, filename, line) << "[protobuf] " << message << std::endl;
+  if (level == LOGLEVEL_ERROR && linpipe::logging_level <= linpipe::LOGGING_ERROR)
+    linpipe::logging_start(linpipe::LOGGING_ERROR, filename, line) << "[protobuf] " << message << std::endl;
+  if (level == LOGLEVEL_FATAL && linpipe::logging_level <= linpipe::LOGGING_FATAL)
+    linpipe::logging_start(linpipe::LOGGING_FATAL, filename, line) << "[protobuf] " << message << std::endl;
+}
+
+static LogHandler* log_handler_ = &LinpipeLogHandler;
 static std::atomic<int> log_silencer_count_ = ATOMIC_VAR_INIT(0);

 LogMessage& LogMessage::operator<<(const std::string& value) {
EOF

# Change one sprintf into snprintf to avoid warnings
patch sentencepiece/third_party/protobuf-lite/strutil.cpp <<"EOF"
--- strutil.cpp.ori	2023-10-18 13:25:11.973899963 +0200
+++ strutil.cpp	2023-10-18 13:25:21.778031257 +0200
@@ -501,9 +501,9 @@
         if ((!utf8_safe || static_cast<uint8>(*src) < 0x80) &&
             (!isprint(*src) ||
              (last_hex_escape && isxdigit(*src)))) {
-          if (dest_len - used < 4) // need space for 4 letter escape
+          if (dest_len - used < 5) // need space for 4 letter escape
             return -1;
-          sprintf(dest + used, (use_hex ? "\\x%02x" : "\\%03o"),
+          snprintf(dest + used, 5, (use_hex ? "\\x%02x" : "\\%03o"),
                   static_cast<uint8>(*src));
           is_hex_escape = use_hex;
           used += 4;
EOF

# Create the Makefile.include
(
grep '^#' ../Makefile.include.sh
cat <<"EOF"

$(call obj,lib/sentencepiece/%): C_FLAGS := $(filter-out $(default_warnings) $(treat_warnings_as_errors),$(C_FLAGS))
$(call obj,lib/sentencepiece/%): C_FLAGS += $(if $(filter win-vs-%,$(PLATFORM)),/wd4244)

EOF
find sentencepiece -name "*.cpp" | sort -u | sed 's@^@LINPIPE_OBJECTS += lib/@; s@[.]cpp$@@'
) >sentencepiece/Makefile.include

rm -rf sentencepiece_git
