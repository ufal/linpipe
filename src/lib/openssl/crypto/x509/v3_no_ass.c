/*
 * Copyright 2023 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include "lib/openssl/include/internal/cryptlib.h"
#include "lib/openssl/include/openssl/asn1.h"
#include "lib/openssl/include/openssl/asn1t.h"
#include "lib/openssl/include/openssl/x509v3.h"
#include "lib/openssl/crypto/x509/ext_dat.h"

static int i2r_NO_ASSERTION(X509V3_EXT_METHOD *method,
                            void *su, BIO *out,
                            int indent)
{
    return 1;
}

static void *r2i_NO_ASSERTION(X509V3_EXT_METHOD *method,
                              X509V3_CTX *ctx, const char *value)
{
    return ASN1_NULL_new();
}

static char *i2s_NO_ASSERTION(const X509V3_EXT_METHOD *method, void *val)
{
    return OPENSSL_strdup("NULL");
}

static void *s2i_NO_ASSERTION(const X509V3_EXT_METHOD *method, X509V3_CTX *ctx, const char *str)
{
    return ASN1_NULL_new();
}

/*
 * The noAssertion X.509v3 extension is defined in ITU Recommendation X.509
 * (2019), Section 17.5.2.7. See: https://www.itu.int/rec/T-REC-X.509-201910-I/en.
 */
const X509V3_EXT_METHOD ossl_v3_no_assertion = {
    NID_no_assertion, 0, ASN1_ITEM_ref(ASN1_NULL),
    0, 0, 0, 0,
    (X509V3_EXT_I2S)i2s_NO_ASSERTION,
    (X509V3_EXT_S2I)s2i_NO_ASSERTION,
    0, 0,
    (X509V3_EXT_I2R)i2r_NO_ASSERTION,
    (X509V3_EXT_R2I)r2i_NO_ASSERTION,
    NULL
};
