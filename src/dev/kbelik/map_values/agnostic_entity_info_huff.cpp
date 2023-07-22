#include <cstring>
#include <filesystem>

#include "common.h"

#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/map_values/bits.h"
#include "dev/kbelik/map_values/bytes_vli.h"
#include "dev/kbelik/map_values/typed_value.h"
#include "dev/kbelik/map_values/vli.h"

#include "dev/kbelik/agnostic_entity_info.h"
#include "dev/kbelik/named_entity.h"
#include "dev/kbelik/typed_value.h"
#include "dev/kbelik/utils.h"

namespace linpipe::kbelik::map_values {

size_t AgnosticEntityInfoH::length(const byte* ptr) const {
  return bytes_vli.length(ptr);
}

size_t AgnosticEntityInfoH::length(const Type& value) const {
  vector<byte> serialized;
  serialize(value, serialized);
  return bytes_vli.length(serialized.data());
}

void AgnosticEntityInfoH::serialize(const Type& value, vector<byte>& data) const {
  vector<byte> result;


  size_t claims_cnt = value.claims.size();
  vector<byte> cnt_encoded;
  vli.serialize(claims_cnt, result);


  for (auto &[key, vaeip]: value.claims) {
    vector<byte> key_encoded;
    vector<byte> key_huffed;
    huffman.encode(key, key_huffed);
    bytes_vli.serialize(key_huffed, result);

    vector<byte> aeip_cnt_encoded;
    vli.serialize(vaeip.size(), result);

    for (auto &aeip : vaeip) {
      vector<byte> aeip_encoded;
      encodeAEIP(aeip, aeip_encoded);


      result.insert(result.end(), aeip_encoded.begin(), aeip_encoded.end());
    }
  }

  vector<byte> ne_encoded;
  encodeNE(value.named_entities, ne_encoded);
  result.insert(result.end(), ne_encoded.begin(), ne_encoded.end());

  vector<byte> fictional_encoded;
  int fictional = static_cast<int>(value.fictional);
  vli.serialize(fictional, result);

  bytes_vli.serialize(result, data);
}

void AgnosticEntityInfoH::deserialize(const byte*& ptr_whole, Type& value) const {
  vector<byte> result;
  bytes_vli.deserialize(ptr_whole, result);

  const byte* ptr = result.data();

  uint64_t claims_cnt;
  vli.deserialize(ptr, claims_cnt);

  unordered_map<string, vector<AEIProperties>> claims;

  for (size_t i = 0; i < claims_cnt; ++i) {
    string key;
    vector<byte> key_bytes;
    bytes_vli.deserialize(ptr, key_bytes);
    huffman.decode(key_bytes.data(), key);

    uint64_t aeip_cnt;
    vli.deserialize(ptr, aeip_cnt);

    vector<AEIProperties> vaeip;
    for (size_t j = 0; j < aeip_cnt; ++j) {
      AEIProperties aeip;
      decodeAEIP(ptr, aeip);

      vaeip.push_back(aeip);
    }

    claims[key] = vaeip;
  }

  vector<NamedEntity> ne;
  decodeNE(ptr, ne);

  uint64_t fictional_as_num;
  vli.deserialize(ptr, fictional_as_num);
  Ternary fictional = static_cast<Ternary>(fictional_as_num);

  value.claims = claims;
  value.named_entities = ne;
  value.fictional = fictional;

}

void AgnosticEntityInfoH::encodeAEIP(const AEIProperties& aeip, vector<byte>& encoded) const {
  vector<byte> tv_encoded;
  tv.serialize(aeip.tv, tv_encoded);

  uint64_t optionals_cnt = aeip.optionals.size();
  vector<byte> cnt_encoded;
  vli.serialize(optionals_cnt, cnt_encoded);

  vector<byte> result;

  result.insert(result.end(), tv_encoded.begin(), tv_encoded.end());
  result.insert(result.end(), cnt_encoded.begin(), cnt_encoded.end());

  for (auto &[key, vtv] : aeip.optionals) {
    vector<byte> key_encoded;
    vector<byte> key_huffed;
    huffman.encode(key, key_huffed);
    bytes_vli.serialize(key_huffed, key_encoded);
    result.insert(result.end(), key_encoded.begin(), key_encoded.end());

    vector<byte> vtv_cnt_encoded;
    vli.serialize((uint64_t)vtv.size(), vtv_cnt_encoded);
    result.insert(result.end(), vtv_cnt_encoded.begin(), vtv_cnt_encoded.end());

    for (auto &val : vtv) {
      vector<byte> val_encoded;
      tv.serialize(val, val_encoded);
      result.insert(result.end(), val_encoded.begin(), val_encoded.end());
    }
  }
  bytes_vli.serialize(result, encoded);
}

void AgnosticEntityInfoH::decodeAEIP(const byte*& ptr_whole, AEIProperties& aeip) const {
  vector<byte> result;
  bytes_vli.deserialize(ptr_whole, result);

  const byte* ptr = result.data();

  tv.deserialize(ptr, aeip.tv);

  uint64_t optionals_cnt;
  vli.deserialize(ptr, optionals_cnt);

  unordered_map<string, vector<linpipe::kbelik::TypedValue>> optionals;
  for (size_t i = 0; i < optionals_cnt; ++i) {
    string key;
    vector<byte> key_bytes;
    bytes_vli.deserialize(ptr, key_bytes);
    huffman.decode(key_bytes.data(), key);

    uint64_t vtv_cnt;
    vli.deserialize(ptr, vtv_cnt);

    vector<linpipe::kbelik::TypedValue> vtv;

    for (size_t j = 0; j < vtv_cnt; ++j) {
      linpipe::kbelik::TypedValue tv_result;
      tv.deserialize(ptr, tv_result);

      vtv.push_back(tv_result);
    }

    optionals[key] = vtv;
  }
  aeip.optionals = optionals;
}

void AgnosticEntityInfoH::encodeNE(const vector<NamedEntity>& value, vector<byte>& encoded) const {
  auto ne_as_bits = NamedEntityConverter::ne_to_bools(value);
  bits.serialize(ne_as_bits, encoded);
}

void AgnosticEntityInfoH::decodeNE(const byte*& ptr, vector<NamedEntity>& ne) const {
  vector<bool> ne_as_bits;
  bits.deserialize(ptr, ne_as_bits);
  ne = NamedEntityConverter::bools_to_ne(ne_as_bits);
}

} // namespace linpipe::kbelik::map_values
