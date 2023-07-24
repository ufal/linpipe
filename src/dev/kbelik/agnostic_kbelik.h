#pragma once

#include "common.h"

#include "dev/kbelik/general_kbelik.h"

#include "dev/kbelik/map_keys/qid8.h"
#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"

namespace linpipe::kbelik {

class AgnosticKbelik : public GeneralKbelik<map_keys::QID8, map_values::AgnosticEntityInfoH> {
 public:
  AgnosticKbelik(filesystem::path map_path, size_t offset=0, int64_t length=-1);
  static inline void build(istream& jsons, ostream& result);
 private:
  NamedEntityMapper nem;


  static inline void build_nem(istream& jsons, NamedEntityMapper& nem);
  static inline void build_map(istream& jsons, DynamicMap<map_keys::QID8, map_values::AgnosticEntityInfoH>& dm);

  size_t load_nem (size_t offset);
  virtual void load_map (size_t offset, int64_t length) override;
};

void AgnosticKbelik::build(istream& jsons, ostream& result) {
  HuffmanTree huff = HuffmanTree();
  GeneralKbelik::build_huffman(jsons, huff);

  auto nem = NamedEntityMapper();
  AgnosticKbelik::build_nem(jsons, nem);
  auto mv = map_values::AgnosticEntityInfoH(huff, nem);

  auto mk = map_keys::QID8();

  auto dm = DynamicMap<map_keys::QID8, map_values::AgnosticEntityInfoH>(mk, mv);
  AgnosticKbelik::build_map(jsons, dm);

  vector<byte> huff_serialized;
  huff.serialize(huff_serialized);
  result.write(reinterpret_cast<const char*>(huff_serialized.data()), huff_serialized.size());

  vector<byte> nem_serialized;
  nem.serialize(nem_serialized);
  result.write(reinterpret_cast<const char*>(nem_serialized.data()), nem_serialized.size());

  dm.save_map(result, test);
}

void AgnosticKbelik::build_map(istream& jsons, DynamicMap<map_keys::QID8, map_values::AgnosticEntityInfoH>& dm) {
  string line;
  while (getline(jsons, line)) {
    auto js = Json::parse(line);
    string q_str = js["qid"];
    auto id = ID(q_str);
    auto aei = map_values::AgnosticEntityInfoH::Type(js);
    dm.add(id, aei);
  }
}

void AgnosticKbelik::build_nem(istream& jsons, NamedEntityMapper& nem) {
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

AgnosticKbelik::AgnosticKbelik(filesystem::path kbelik_path, size_t offset, int64_t /*length*/) {
  this->kbelik_path = kbelik_path;
  size_t huffman_bytes = load_huffman(offset);
  size_t nem_bytes = load_nem(offset + huffman_bytes);
  load_map(offset + huffman_bytes + nem_bytes, -1);
}

size_t AgnosticKbelik::load_nem(size_t offset) {
  size_t nem_size = 0;

  std::ifstream ifs(kbelik_path, std::ios::binary | std::ios::in);

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

void AgnosticKbelik::load_map(size_t offset, int64_t length) {
  auto mv = map_values::AgnosticEntityInfoH(huffman, nem);
  auto mk = map_keys::QID8();
  map = new PersistentMap<map_keys::QID8, map_values::AgnosticEntityInfoH>(kbelik_path, mk, mv, offset, length);
}

} // namespace linpipe::kbelik
