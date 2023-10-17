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

git clone --depth=1 --branch=v0.14.1 https://github.com/yhirose/cpp-httplib cpp-httplib

sed 's/namespace httplib/namespace linpipe::httplib/' cpp-httplib/httplib.h >httplib.h

patch httplib.h <<EOF
--- httplib.h.ori	2023-10-17 21:42:28.410369628 +0200
+++ httplib.h	2023-10-17 21:42:28.422369757 +0200
@@ -257,15 +257,17 @@
 #endif // TARGET_OS_OSX
 #endif // _WIN32
 
-#include <openssl/err.h>
-#include <openssl/evp.h>
-#include <openssl/ssl.h>
-#include <openssl/x509v3.h>
+#include "lib/openssl/include/openssl/err.h"
+#include "lib/openssl/include/openssl/evp.h"
+#include "lib/openssl/include/openssl/ssl.h"
+#include "lib/openssl/include/openssl/x509v3.h"
 
 #if defined(_WIN32) && defined(OPENSSL_USE_APPLINK)
-#include <openssl/applink.c>
+#include "lib/openssl/include/openssl/applink.c"
 #endif
 
+#include "lib/httplib_bundled_certs.h"
+
 #include <iostream>
 #include <sstream>
 
@@ -8529,6 +8531,7 @@
       loaded = detail::load_system_certs_on_macos(SSL_CTX_get_cert_store(ctx_));
 #endif // TARGET_OS_OSX
 #endif // _WIN32
+      if (!loaded) loaded = detail::load_bundled_certs(SSL_CTX_get_cert_store(ctx_));
       if (!loaded) { SSL_CTX_set_default_verify_paths(ctx_); }
     }
   });
EOF

rm -rf cpp-httplib
