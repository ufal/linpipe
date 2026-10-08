// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/mbedtls/include/psa/crypto.h"
#include "lib/sha256.h"

namespace linpipe {

static_assert(PSA_HASH_LENGTH(PSA_ALG_SHA_256) == SHA256::digest_size);

struct SHA256::Impl {
  psa_hash_operation_t operation = psa_hash_operation_init();

  ~Impl() { psa_hash_abort(&operation); }
};

SHA256::SHA256() : impl(std::make_unique<Impl>()) {
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS)
    throw LinpipeError{"SHA256: Cannot initialize PSA crypto, error ", std::to_string(status)};

  status = psa_hash_setup(&impl->operation, PSA_ALG_SHA_256);
  if (status != PSA_SUCCESS)
    throw LinpipeError{"SHA256: Cannot set up SHA-256, error ", std::to_string(status)};
}

SHA256::~SHA256() = default;
SHA256::SHA256(SHA256&&) noexcept = default;
SHA256& SHA256::operator=(SHA256&&) noexcept = default;

SHA256& SHA256::update(std::string_view data) {
  return update(std::as_bytes(std::span(data.data(), data.size())));
}

SHA256& SHA256::update(std::span<const std::byte> data) {
  if (data.empty()) return *this;

  psa_status_t status = psa_hash_update(&impl->operation, reinterpret_cast<const uint8_t*>(data.data()), data.size());
  if (status != PSA_SUCCESS)
    throw LinpipeError{"SHA256::update: Cannot update SHA-256, error ", std::to_string(status)};

  return *this;
}

std::array<std::byte, SHA256::digest_size> SHA256::digest() const {
  psa_hash_operation_t clone = psa_hash_operation_init();
  psa_status_t status = psa_hash_clone(&impl->operation, &clone);
  if (status != PSA_SUCCESS) {
    psa_hash_abort(&clone);
    throw LinpipeError{"SHA256::digest: Cannot clone SHA-256, error ", std::to_string(status)};
  }

  std::array<std::byte, SHA256::digest_size> hash;
  size_t hash_length = 0;
  status = psa_hash_finish(&clone, reinterpret_cast<uint8_t*>(hash.data()), hash.size(), &hash_length);
  psa_hash_abort(&clone);
  if (status != PSA_SUCCESS || hash_length != digest_size)
    throw LinpipeError{"SHA256::digest: Cannot compute SHA-256, error ", std::to_string(status)};

  return hash;
}

std::string SHA256::hexdigest() const {
  static constexpr char hex[] = "0123456789abcdef";

  std::string result;
  result.reserve(2 * digest_size);
  for (auto byte : digest()) {
    auto value = std::to_integer<unsigned>(byte);
    result.push_back(hex[value >> 4]);
    result.push_back(hex[value & 0xF]);
  }
  return result;
}

} // namespace linpipe
