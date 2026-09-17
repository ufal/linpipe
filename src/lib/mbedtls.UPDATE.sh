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

MBEDTLS=mbedtls-4.2.0
rm -rf $MBEDTLS/
curl -L https://github.com/Mbed-TLS/mbedtls/releases/download/$MBEDTLS/$MBEDTLS.tar.bz2 | tar xj

rm -rf mbedtls/include/ mbedtls/src/

get() {
  tgt=mbedtls/"$1"
  src=$MBEDTLS/"$2"
  shift 2
  mkdir -p "$tgt"
  for f in "$@"; do cp "$src/$f" "$tgt/$f"; done
}

get src library \
  debug.c error.c ssl_ciphersuites.c ssl_client.c ssl_debug_helpers_generated.c \
  ssl_msg.c ssl_tls.c ssl_tls12_client.c ssl_tls12_server.c ssl_tls13_client.c \
  ssl_tls13_generic.c ssl_tls13_keys.c ssl_tls13_server.c x509.c x509_crt.c \
  x509_oid.c

get src tf-psa-crypto/core \
  psa_crypto.c psa_crypto_client.c psa_crypto_driver_wrappers_no_static.c \
  psa_crypto_random.c psa_crypto_slot_management.c psa_crypto_storage.c \
  psa_its_file.c psa_util.c

get src tf-psa-crypto/drivers/builtin/src \
  aes.c aesce.c aesni.c aria.c bignum.c bignum_core.c camellia.c ccm.c chacha20.c \
  chacha20_neon.c chachapoly.c cipher.c cipher_wrap.c cmac.c ctr_drbg.c ecdsa.c \
  ecjpake.c ecp.c ecp_curves.c entropy.c entropy_poll.c gcm.c hmac_drbg.c md5.c \
  poly1305.c psa_crypto_aead.c psa_crypto_cipher.c psa_crypto_ecp.c \
  psa_crypto_ffdh.c psa_crypto_hash.c psa_crypto_mac.c psa_crypto_pake.c \
  psa_crypto_rsa.c psa_crypto_xof.c psa_util_internal.c ripemd160.c rsa.c \
  rsa_alt_helpers.c sha1.c sha256.c sha3.c sha512.c

get src tf-psa-crypto/extras \
  md.c pk.c pk_ecc.c pk_rsa.c pk_wrap.c pkparse.c pkwrite.c

get src tf-psa-crypto/platform \
  platform_util.c

get src tf-psa-crypto/utilities \
  asn1parse.c asn1write.c base64.c constant_time.c oid.c pem.c pkcs5.c

get include/mbedtls include/mbedtls \
  build_info.h debug.h error.h mbedtls_config.h net_sockets.h oid.h pkcs7.h \
  ssl.h ssl_ciphersuites.h version.h x509.h x509_crl.h x509_crt.h

get include/mbedtls/private include/mbedtls/private \
  config_adjust_ssl.h config_adjust_x509.h

get include library \
  debug_internal.h mbedtls_common.h mbedtls_platform_requirements.h \
  mbedtls_utils.h ssl_ciphersuites_internal.h ssl_client.h ssl_debug_helpers.h \
  ssl_misc.h ssl_tls13_invasive.h ssl_tls13_keys.h x509_internal.h x509_oid.h

get include tf-psa-crypto/core \
  alignment.h check_crypto_config.h psa_crypto_core.h psa_crypto_core_common.h \
  psa_crypto_driver_wrappers.h psa_crypto_invasive.h psa_crypto_its.h \
  psa_crypto_random.h psa_crypto_random_impl.h psa_crypto_slot_management.h \
  psa_crypto_storage.h tf_psa_crypto_common.h \
  tf_psa_crypto_platform_requirements.h

get include tf-psa-crypto/dispatch \
  psa_crypto_driver_wrappers_no_static.h

get include/mbedtls/private tf-psa-crypto/drivers/builtin/include/mbedtls/private \
  aes.h aria.h bignum.h camellia.h ccm.h chacha20.h chachapoly.h cipher.h \
  cmac.h crypto_adjust_config_enable_builtins.h \
  crypto_adjust_config_tweak_builtins.h crypto_builtin_composites.h \
  crypto_builtin_key_derivation.h crypto_builtin_primitives.h ctr_drbg.h \
  ecdsa.h ecjpake.h ecp.h entropy.h error_common.h gcm.h hmac_drbg.h md5.h \
  pkcs5.h poly1305.h ripemd160.h rsa.h sha1.h sha256.h sha3.h sha512.h

get include/mbedtls tf-psa-crypto/drivers/builtin/include/mbedtls \
  private_access.h

get include tf-psa-crypto/drivers/builtin/src \
  aesce.h aesni.h bignum_core.h bignum_core_invasive.h bignum_internal.h \
  bignum_mod.h bn_mul.h chacha20_neon.h cipher_invasive.h cipher_wrap.h ctr.h \
  ecp_invasive.h entropy_poll.h md_psa.h psa_crypto_aead.h psa_crypto_cipher.h \
  psa_crypto_ecp.h psa_crypto_ffdh.h psa_crypto_hash.h psa_crypto_mac.h \
  psa_crypto_pake.h psa_crypto_rsa.h psa_crypto_xof.h psa_util_internal.h \
  rsa_alt_helpers.h rsa_internal.h rsa_invasive.h

get include tf-psa-crypto/extras \
  md_wrap.h pk_internal.h pk_wrap.h pkwrite.h

get include/mbedtls tf-psa-crypto/include/mbedtls \
  asn1.h asn1write.h base64.h compat-3-crypto.h constant_time.h lms.h md.h \
  pem.h pk.h platform.h platform_time.h platform_util.h psa_util.h threading.h

get include/mbedtls/private tf-psa-crypto/include/mbedtls/private \
  pk_private.h

get include/psa tf-psa-crypto/include/psa \
  crypto.h crypto_compat.h crypto_config.h crypto_driver_common.h \
  crypto_driver_contexts_composites.h crypto_driver_contexts_key_derivation.h \
  crypto_driver_contexts_primitives.h crypto_driver_random.h crypto_extra.h \
  crypto_platform.h crypto_sizes.h crypto_struct.h crypto_types.h \
  crypto_values.h

get include/tf-psa-crypto tf-psa-crypto/include/tf-psa-crypto \
  build_info.h

get include/tf-psa-crypto/private tf-psa-crypto/include/tf-psa-crypto/private \
  crypto_adjust_config_auto_enabled.h crypto_adjust_config_dependencies.h \
  crypto_adjust_config_derived.h crypto_adjust_config_key_pair_types.h \
  crypto_adjust_config_support.h crypto_adjust_config_synonyms.h

get include tf-psa-crypto/platform \
  threading_internal.h

get include tf-psa-crypto/utilities \
  base64_internal.h constant_time_impl.h constant_time_internal.h crypto_oid.h

# Fix include paths
(
  echo 's@^#include [<"]\(\(mbedtls\|psa\|tf-psa-crypto\)[^">]*\)[">]@#include "lib/mbedtls/include/\\1"@'
  find mbedtls/ -type f | while read path; do echo 's@^#include "'"$(basename "$path")"'"@#include "lib/'"$path"'"@'; done
) >mbedtls.UPDATE.sh.sed
sed -f mbedtls.UPDATE.sh.sed $(find mbedtls/ -type f) -i
rm -rf mbedtls.UPDATE.sh.sed

rm -rf $MBEDTLS
