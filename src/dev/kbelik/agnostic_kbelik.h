#pragma once

#include "common.h"

#include "dev/kbelik/general_kbelik.h"

#include "dev/kbelik/map_keys/qid8.h"
#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"

namespace linpipe::kbelik {

template<typename MapKey>
class AgnosticKbelik : public GeneralKbelik<MapKey, map_values::AgnosticEntityInfoH> {
 public:
  AgnosticKbelik(filesystem::path map_path, size_t offset=0, int64_t length=-1);
  static inline void build(istream& jsons, ostream& result);
 private:
  NamedEntityMapper nem;


  static inline void build_nem(istream& jsons, NamedEntityMapper& nem);
  static inline void build_map(istream& jsons, DynamicMap<MapKey, map_values::AgnosticEntityInfoH>& dm);

  size_t load_nem (size_t offset);
  virtual void load_map (size_t offset, int64_t length) override;
};

template<typename MapKey>
void AgnosticKbelik<MapKey>::build(istream& jsons, ostream& result) {
  HuffmanTree huff = HuffmanTree();
  GeneralKbelik<MapKey, map_values::AgnosticEntityInfoH>::build_huffman(jsons, huff);

  auto nem = NamedEntityMapper();
  AgnosticKbelik::build_nem(jsons, nem);
  auto mv = map_values::AgnosticEntityInfoH(huff, nem);

  auto mk = MapKey();

  auto dm = DynamicMap<MapKey, map_values::AgnosticEntityInfoH>(mk, mv);
  AgnosticKbelik::build_map(jsons, dm);

  vector<byte> huff_serialized;
  huff.serialize(huff_serialized);
  result.write(reinterpret_cast<const char*>(huff_serialized.data()), huff_serialized.size());

  vector<byte> nem_serialized;
  nem.serialize(nem_serialized);
  result.write(reinterpret_cast<const char*>(nem_serialized.data()), nem_serialized.size());

  dm.save_map(result, test);
}

template<typename MapKey>
void AgnosticKbelik<MapKey>::build_map(istream& jsons, DynamicMap<MapKey, map_values::AgnosticEntityInfoH>& dm) {
  string line;
  while (getline(jsons, line)) {
    auto js = Json::parse(line);
    string q_str = js["qid"];
    auto id = ID(q_str);
    auto aei = map_values::AgnosticEntityInfoH::Type(js);
    dm.add(id, aei);
  }
}

template<typename MapKey>
void AgnosticKbelik<MapKey>::build_nem(istream& jsons, NamedEntityMapper& nem) {
  auto remember_pos = jsons.tellg();

  string line;
  while (getline(jsons, line)) {
    auto js = Json::parse(line);
    nem.add_entities_from_json(js);
  }

  nem.sort();

  jsons.clear();
  jsons.seekg(remember_pos);
}

template<typename MapKey>
AgnosticKbelik<MapKey>::AgnosticKbelik(filesystem::path kbelik_path, size_t offset, int64_t /*length*/) {
  this->kbelik_path = kbelik_path;
  size_t huffman_bytes = this->load_huffman(offset);
  size_t nem_bytes = load_nem(offset + huffman_bytes);
  load_map(offset + huffman_bytes + nem_bytes, -1);
}

template<typename MapKey>
size_t AgnosticKbelik<MapKey>::load_nem(size_t offset) {
  size_t nem_size = 0;

  std::ifstream ifs(this->kbelik_path, std::ios::binary | std::ios::in);

  nem = NamedEntityMapper();

  if (ifs.is_open()) {
    ifs.seekg(offset);
    nem_size = nem.deserialize(ifs);
    ifs.close();
  } 
  else {
    throw LinpipeError("Failed to open file for reading.\n");
  }

  return nem_size;
}

template<typename MapKey>
void AgnosticKbelik<MapKey>::load_map(size_t offset, int64_t length) {
  auto mv = map_values::AgnosticEntityInfoH(this->huffman, nem);
  auto mk = MapKey();
  this->map = new PersistentMap<MapKey, map_values::AgnosticEntityInfoH>(this->kbelik_path, mk, mv, offset, length);
}

} // namespace linpipe::kbelik
