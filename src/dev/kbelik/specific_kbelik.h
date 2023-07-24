#pragma once

#include "common.h"

#include "dev/kbelik/general_kbelik.h"

#include "dev/kbelik/map_values/specific_entity_info_huff.h"
#include "dev/kbelik/map_keys/qid4.h"

namespace linpipe::kbelik {

template<typename MapKey>
class SpecificKbelik : public GeneralKbelik<MapKey, map_values::SpecificEntityInfoH> {
 public:
  SpecificKbelik(filesystem::path map_path, size_t offset=0, int64_t length=-1);
  static inline void build(istream& jsons, ostream& result);

 private:
  static inline void build_map(istream& jsons, DynamicMap<MapKey, map_values::SpecificEntityInfoH>& dm);
  virtual void load_map (size_t offset, int64_t length) override;
};

template<typename MapKey>
void SpecificKbelik<MapKey>::build(istream& jsons, ostream& result) {
  HuffmanTree huff = HuffmanTree();
  GeneralKbelik<MapKey, map_values::SpecificEntityInfoH>::build_huffman(jsons, huff);

  auto mv = map_values::SpecificEntityInfoH(huff);

  auto mk = MapKey();

  auto dm = DynamicMap<MapKey, map_values::SpecificEntityInfoH>(mk, mv);
  SpecificKbelik<MapKey>::build_map(jsons, dm);

  vector<byte> huff_serialized;
  huff.serialize(huff_serialized);
  result.write(reinterpret_cast<const char*>(huff_serialized.data()), huff_serialized.size());

  dm.save_map(result, test);
}

template<typename MapKey>
SpecificKbelik<MapKey>::SpecificKbelik(filesystem::path kbelik_path, size_t offset, int64_t /*length*/) {
  this->kbelik_path = kbelik_path;
  size_t huffman_bytes = this->load_huffman(offset);
  load_map(offset + huffman_bytes, -1);
}

template<typename MapKey>
void SpecificKbelik<MapKey>::build_map(istream& jsons, DynamicMap<MapKey, map_values::SpecificEntityInfoH>& dm) {
  string line;
  while (getline(jsons, line)) {
    auto js = Json::parse(line);
    string q_str = js["qid"];
    auto id = ID(q_str);
    auto sei = map_values::SpecificEntityInfoH::Type(js);
    dm.add(id, sei);
  }
}

template<typename MapKey>
void SpecificKbelik<MapKey>::load_map(size_t offset, int64_t length) {
  auto mv = map_values::SpecificEntityInfoH(this->huffman);
  auto mk = MapKey();
  this->map = new PersistentMap<MapKey, map_values::SpecificEntityInfoH>(this->kbelik_path, mk, mv, offset, length);
}
 
} // linpipe::kbelik
