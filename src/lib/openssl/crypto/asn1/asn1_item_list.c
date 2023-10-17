/*
 * Copyright 2000-2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/* We need to use the low level ASN1 items until they are removed */
#define OPENSSL_SUPPRESS_DEPRECATED

#include <stdio.h>
#include "lib/openssl/include/internal/cryptlib.h"
#include "lib/openssl/include/openssl/asn1.h"
#include "lib/openssl/include/openssl/asn1t.h"
#include "lib/openssl/include/openssl/cms.h"
#include "lib/openssl/include/openssl/dh.h"
#include "lib/openssl/include/openssl/ocsp.h"
#include "lib/openssl/include/openssl/pkcs7.h"
#include "lib/openssl/include/openssl/pkcs12.h"
#include "lib/openssl/include/openssl/rsa.h"
#include "lib/openssl/include/openssl/x509v3.h"

#include "lib/openssl/crypto/asn1/asn1_item_list.h"

const ASN1_ITEM *ASN1_ITEM_lookup(const char *name)
{
    size_t i;

    for (i = 0; i < OSSL_NELEM(asn1_item_list); i++) {
        const ASN1_ITEM *it = ASN1_ITEM_ptr(asn1_item_list[i]);

        if (strcmp(it->sname, name) == 0)
            return it;
    }
    return NULL;
}

const ASN1_ITEM *ASN1_ITEM_get(size_t i)
{
    if (i >= OSSL_NELEM(asn1_item_list))
        return NULL;
    return ASN1_ITEM_ptr(asn1_item_list[i]);
}
