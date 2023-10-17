/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "lib/openssl/include/openssl/err.h"
#include "lib/openssl/include/openssl/conferr.h"
#include "lib/openssl/include/crypto/conferr.h"

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA CONF_str_reasons[] = {
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_ERROR_LOADING_DSO), "error loading dso"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_INVALID_PRAGMA), "invalid pragma"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_LIST_CANNOT_BE_NULL),
    "list cannot be null"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_MANDATORY_BRACES_IN_VARIABLE_EXPANSION),
    "mandatory braces in variable expansion"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_MISSING_CLOSE_SQUARE_BRACKET),
    "missing close square bracket"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_MISSING_EQUAL_SIGN),
    "missing equal sign"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_MISSING_INIT_FUNCTION),
    "missing init function"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_MODULE_INITIALIZATION_ERROR),
    "module initialization error"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_NO_CLOSE_BRACE), "no close brace"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_NO_CONF), "no conf"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_NO_CONF_OR_ENVIRONMENT_VARIABLE),
    "no conf or environment variable"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_NO_SECTION), "no section"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_NO_SUCH_FILE), "no such file"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_NO_VALUE), "no value"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_NUMBER_TOO_LARGE), "number too large"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_OPENSSL_CONF_REFERENCES_MISSING_SECTION),
    "openssl conf references missing section"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_RECURSIVE_DIRECTORY_INCLUDE),
    "recursive directory include"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_RELATIVE_PATH), "relative path"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_SSL_COMMAND_SECTION_EMPTY),
    "ssl command section empty"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_SSL_COMMAND_SECTION_NOT_FOUND),
    "ssl command section not found"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_SSL_SECTION_EMPTY), "ssl section empty"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_SSL_SECTION_NOT_FOUND),
    "ssl section not found"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_UNABLE_TO_CREATE_NEW_SECTION),
    "unable to create new section"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_UNKNOWN_MODULE_NAME),
    "unknown module name"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_VARIABLE_EXPANSION_TOO_LONG),
    "variable expansion too long"},
    {ERR_PACK(ERR_LIB_CONF, 0, CONF_R_VARIABLE_HAS_NO_VALUE),
    "variable has no value"},
    {0, NULL}
};

#endif

int ossl_err_load_CONF_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_reason_error_string(CONF_str_reasons[0].error) == NULL)
        ERR_load_strings_const(CONF_str_reasons);
#endif
    return 1;
}
