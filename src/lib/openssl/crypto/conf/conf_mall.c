/*
 * Copyright 2002-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/* We need to use some engine deprecated APIs */
#define OPENSSL_SUPPRESS_DEPRECATED

#include <stdio.h>
#include "lib/openssl/include/openssl/crypto.h"
#include "lib/openssl/include/internal/cryptlib.h"
#include "lib/openssl/include/openssl/conf.h"
#include "lib/openssl/include/openssl/x509.h"
#include "lib/openssl/include/openssl/asn1.h"
#include "lib/openssl/include/openssl/engine.h"
#include "lib/openssl/include/internal/provider.h"
#include "lib/openssl/include/crypto/rand.h"
#include "lib/openssl/crypto/conf/conf_local.h"

/* Load all OpenSSL builtin modules */

void OPENSSL_load_builtin_modules(void)
{
    /* Add builtin modules here */
    ASN1_add_oid_module();
    ASN1_add_stable_module();
#ifndef OPENSSL_NO_ENGINE
    ENGINE_add_conf_module();
#endif
    EVP_add_alg_module();
    ossl_config_add_ssl_module();
    ossl_provider_add_conf_module();
    ossl_random_add_conf_module();
}
