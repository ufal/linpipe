/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2022 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "lib/openssl/include/openssl/err.h"
#include "lib/openssl/include/openssl/bioerr.h"
#include "lib/openssl/include/crypto/bioerr.h"

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA BIO_str_reasons[] = {
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_ACCEPT_ERROR), "accept error"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_ADDRINFO_ADDR_IS_NOT_AF_INET),
    "addrinfo addr is not af inet"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_AMBIGUOUS_HOST_OR_SERVICE),
    "ambiguous host or service"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_BAD_FOPEN_MODE), "bad fopen mode"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_BROKEN_PIPE), "broken pipe"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_CONNECT_ERROR), "connect error"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_CONNECT_TIMEOUT), "connect timeout"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_GETHOSTBYNAME_ADDR_IS_NOT_AF_INET),
    "gethostbyname addr is not af inet"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_GETSOCKNAME_ERROR), "getsockname error"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_GETSOCKNAME_TRUNCATED_ADDRESS),
    "getsockname truncated address"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_GETTING_SOCKTYPE), "getting socktype"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_INVALID_ARGUMENT), "invalid argument"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_INVALID_SOCKET), "invalid socket"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_IN_USE), "in use"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_LENGTH_TOO_LONG), "length too long"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_LISTEN_V6_ONLY), "listen v6 only"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_LOOKUP_RETURNED_NOTHING),
    "lookup returned nothing"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_MALFORMED_HOST_OR_SERVICE),
    "malformed host or service"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_NBIO_CONNECT_ERROR), "nbio connect error"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_NO_ACCEPT_ADDR_OR_SERVICE_SPECIFIED),
    "no accept addr or service specified"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_NO_HOSTNAME_OR_SERVICE_SPECIFIED),
    "no hostname or service specified"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_NO_PORT_DEFINED), "no port defined"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_NO_SUCH_FILE), "no such file"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_PORT_MISMATCH), "port mismatch"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_TFO_DISABLED), "tfo disabled"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_TFO_NO_KERNEL_SUPPORT),
    "tfo no kernel support"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_TRANSFER_ERROR), "transfer error"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_TRANSFER_TIMEOUT), "transfer timeout"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNABLE_TO_BIND_SOCKET),
    "unable to bind socket"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNABLE_TO_CREATE_SOCKET),
    "unable to create socket"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNABLE_TO_KEEPALIVE),
    "unable to keepalive"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNABLE_TO_LISTEN_SOCKET),
    "unable to listen socket"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNABLE_TO_NODELAY), "unable to nodelay"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNABLE_TO_REUSEADDR),
    "unable to reuseaddr"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNABLE_TO_TFO), "unable to tfo"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNAVAILABLE_IP_FAMILY),
    "unavailable ip family"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNINITIALIZED), "uninitialized"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNKNOWN_INFO_TYPE), "unknown info type"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNSUPPORTED_IP_FAMILY),
    "unsupported ip family"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNSUPPORTED_METHOD), "unsupported method"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_UNSUPPORTED_PROTOCOL_FAMILY),
    "unsupported protocol family"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_WRITE_TO_READ_ONLY_BIO),
    "write to read only BIO"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_WSASTARTUP), "WSAStartup"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_LOCAL_ADDR_NOT_AVAILABLE),
     "local address not available"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_PEER_ADDR_NOT_AVAILABLE),
     "peer address not available"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_NON_FATAL),
     "non-fatal or transient error"},
    {ERR_PACK(ERR_LIB_BIO, 0, BIO_R_PORT_MISMATCH),
     "port mismatch"},
    {0, NULL}
};

#endif

int ossl_err_load_BIO_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_reason_error_string(BIO_str_reasons[0].error) == NULL)
        ERR_load_strings_const(BIO_str_reasons);
#endif
    return 1;
}

#ifndef OPENSSL_NO_SOCK

int BIO_err_is_non_fatal(unsigned int errcode)
{
    if (ERR_SYSTEM_ERROR(errcode))
        return BIO_sock_non_fatal_error(ERR_GET_REASON(errcode));
    else if (ERR_GET_LIB(errcode) == ERR_LIB_BIO
             && ERR_GET_REASON(errcode) == BIO_R_NON_FATAL)
        return 1;
    else
        return 0;
}

#endif
