#pragma once

#include "common.h"

#include "dev/kbelik/general_kbelik.h"

#include "dev/kbelik/map_values/specific_entity_info_huff.h"
#include "dev/kbelik/map_keys/qid4.h"

namespace linpipe::kbelik {

class SpecificKbelik : public GeneralKbelik<map_keys::QID8, map_values::SpecificEntityInfoH> {
 public:
  SpecificKbelik(filesystem::path map_path, size_t offset=0, int64_t length=-1);
  static inline void build(istream& jsons, ostream& result);

 private:
  static inline void build_map(istream& jsons, DynamicMap<map_keys::QID8, map_values::SpecificEntityInfoH>& dm);
  virtual void load_map (size_t offset, int64_t length) override;
};

void SpecificKbelik::build(istream& jsons, ostream& result) {
  HuffmanTree huff = HuffmanTree();
  GeneralKbelik::build_huffman(jsons, huff);

  auto mv = map_values::SpecificEntityInfoH(huff);

  auto mk = map_keys::QID8();

  auto dm = DynamicMap<map_keys::QID8, map_values::SpecificEntityInfoH>(mk, mv);
  SpecificKbelik::build_map(jsons, dm);

  vector<byte> huff_serialized;
  huff.serialize(huff_serialized);
  result.write(reinterpret_cast<const char*>(huff_serialized.data()), huff_serialized.size());

  dm.save_map(result, test);
}

SpecificKbelik::SpecificKbelik(filesystem::path kbelik_path, size_t offset, int64_t /*length*/) {
  this->kbelik_path = kbelik_path;
  size_t huffman_bytes = load_huffman(offset);
  load_map(offset + huffman_bytes, -1);
}

void SpecificKbelik::build_map(istream& jsons, DynamicMap<map_keys::QID8, map_values::SpecificEntityInfoH>& dm) {
  string line;
  while (getline(jsons, line)) {
    auto js = Json::parse(line);
    string q_str = js["qid"];
    auto id = ID(q_str);
    auto sei = map_values::SpecificEntityInfoH::Type(js);
    dm.add(id, sei);
  }
}

void SpecificKbelik::load_map(size_t offset, int64_t length) {
  auto mv = map_values::SpecificEntityInfoH(huffman);
  auto mk = map_keys::QID8();
  map = new PersistentMap<map_keys::QID8, map_values::SpecificEntityInfoH>(kbelik_path, mk, mv, offset, length);
}
 
} // linpipe::kbelik
