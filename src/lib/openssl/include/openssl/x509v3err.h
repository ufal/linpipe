/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2022 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OPENSSL_X509V3ERR_H
# define OPENSSL_X509V3ERR_H
# pragma once

# include "lib/openssl/include/openssl/opensslconf.h"
# include "lib/openssl/include/openssl/symhacks.h"
# include "lib/openssl/include/openssl/cryptoerr_legacy.h"



/*
 * X509V3 reason codes.
 */
# define X509V3_R_BAD_IP_ADDRESS                          118
# define X509V3_R_BAD_OBJECT                              119
# define X509V3_R_BAD_OPTION                              170
# define X509V3_R_BAD_VALUE                               171
# define X509V3_R_BN_DEC2BN_ERROR                         100
# define X509V3_R_BN_TO_ASN1_INTEGER_ERROR                101
# define X509V3_R_DIRNAME_ERROR                           149
# define X509V3_R_DISTPOINT_ALREADY_SET                   160
# define X509V3_R_DUPLICATE_ZONE_ID                       133
# define X509V3_R_EMPTY_KEY_USAGE                         169
# define X509V3_R_ERROR_CONVERTING_ZONE                   131
# define X509V3_R_ERROR_CREATING_EXTENSION                144
# define X509V3_R_ERROR_IN_EXTENSION                      128
# define X509V3_R_EXPECTED_A_SECTION_NAME                 137
# define X509V3_R_EXTENSION_EXISTS                        145
# define X509V3_R_EXTENSION_NAME_ERROR                    115
# define X509V3_R_EXTENSION_NOT_FOUND                     102
# define X509V3_R_EXTENSION_SETTING_NOT_SUPPORTED         103
# define X509V3_R_EXTENSION_VALUE_ERROR                   116
# define X509V3_R_ILLEGAL_EMPTY_EXTENSION                 151
# define X509V3_R_INCORRECT_POLICY_SYNTAX_TAG             152
# define X509V3_R_INVALID_ASNUMBER                        162
# define X509V3_R_INVALID_ASRANGE                         163
# define X509V3_R_INVALID_BOOLEAN_STRING                  104
# define X509V3_R_INVALID_CERTIFICATE                     158
# define X509V3_R_INVALID_EMPTY_NAME                      108
# define X509V3_R_INVALID_EXTENSION_STRING                105
# define X509V3_R_INVALID_INHERITANCE                     165
# define X509V3_R_INVALID_IPADDRESS                       166
# define X509V3_R_INVALID_MULTIPLE_RDNS                   161
# define X509V3_R_INVALID_NAME                            106
# define X509V3_R_INVALID_NULL_ARGUMENT                   107
# define X509V3_R_INVALID_NULL_VALUE                      109
# define X509V3_R_INVALID_NUMBER                          140
# define X509V3_R_INVALID_NUMBERS                         141
# define X509V3_R_INVALID_OBJECT_IDENTIFIER               110
# define X509V3_R_INVALID_OPTION                          138
# define X509V3_R_INVALID_POLICY_IDENTIFIER               134
# define X509V3_R_INVALID_PROXY_POLICY_SETTING            153
# define X509V3_R_INVALID_PURPOSE                         146
# define X509V3_R_INVALID_SAFI                            164
# define X509V3_R_INVALID_SECTION                         135
# define X509V3_R_INVALID_SYNTAX                          143
# define X509V3_R_ISSUER_DECODE_ERROR                     126
# define X509V3_R_MISSING_VALUE                           124
# define X509V3_R_NEED_ORGANIZATION_AND_NUMBERS           142
# define X509V3_R_NEGATIVE_PATHLEN                        168
# define X509V3_R_NO_CONFIG_DATABASE                      136
# define X509V3_R_NO_ISSUER_CERTIFICATE                   121
# define X509V3_R_NO_ISSUER_DETAILS                       127
# define X509V3_R_NO_POLICY_IDENTIFIER                    139
# define X509V3_R_NO_PROXY_CERT_POLICY_LANGUAGE_DEFINED   154
# define X509V3_R_NO_PUBLIC_KEY                           114
# define X509V3_R_NO_SUBJECT_DETAILS                      125
# define X509V3_R_OPERATION_NOT_DEFINED                   148
# define X509V3_R_OTHERNAME_ERROR                         147
# define X509V3_R_POLICY_LANGUAGE_ALREADY_DEFINED         155
# define X509V3_R_POLICY_PATH_LENGTH                      156
# define X509V3_R_POLICY_PATH_LENGTH_ALREADY_DEFINED      157
# define X509V3_R_POLICY_WHEN_PROXY_LANGUAGE_REQUIRES_NO_POLICY 159
# define X509V3_R_SECTION_NOT_FOUND                       150
# define X509V3_R_UNABLE_TO_GET_ISSUER_DETAILS            122
# define X509V3_R_UNABLE_TO_GET_ISSUER_KEYID              123
# define X509V3_R_UNKNOWN_BIT_STRING_ARGUMENT             111
# define X509V3_R_UNKNOWN_EXTENSION                       129
# define X509V3_R_UNKNOWN_EXTENSION_NAME                  130
# define X509V3_R_UNKNOWN_OPTION                          120
# define X509V3_R_UNKNOWN_VALUE                           172
# define X509V3_R_UNSUPPORTED_OPTION                      117
# define X509V3_R_UNSUPPORTED_TYPE                        167
# define X509V3_R_USER_TOO_LONG                           132

#endif
