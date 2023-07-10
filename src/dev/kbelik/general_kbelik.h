#pragma once

#include <filesystem>

#include "lib/json.h"

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/dynamic_map.h"
#include "dev/kbelik/id.h"
#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/utils.h"

#include "dev/kbelik/map_values/id.h"

namespace linpipe::kbelik {

template<typename ValueMV>
class GeneralKbelik {
 public:
  // Rovnou map_path
  GeneralKbelik(filesystem::path map_path, size_t offset=0, int64_t length=-1);
  ~GeneralKbelik();

  bool find(ID id, typename ValueMV::Type& value) const;
  void close(); // Should end mmap.

  bool opened() const;

  static inline void build(istream& jsons, ostream& result);
 private:
  // Chápe PM formát dat?
  filesystem::path kbelik_path;
  ByteSerializerDeserializers bsds;
  PersistentMap<map_values::ID, ValueMV>* map = nullptr;

  size_t load_bsds (size_t offset);
  void load_map (size_t offset, int64_t length);

  static inline void build_bsds(istream& jsons, ByteSerializerDeserializers& bsds);
  static inline void build_map(istream& jsons, DynamicMap<map_values::ID, ValueMV>& dm);
};

template<typename ValueMV>
void GeneralKbelik<ValueMV>::build(istream& jsons, ostream& result) {
  ByteSerializerDeserializers bsds;
  GeneralKbelik::build_bsds(jsons, bsds);
  
  auto dm = DynamicMap<map_values::ID, ValueMV>(&bsds);
  GeneralKbelik::build_map(jsons, dm);

  vector<byte> huff_serialized;
  bsds.huffman.serialize(huff_serialized);
  result.write(reinterpret_cast<const char*>(huff_serialized.data()), huff_serialized.size());

  dm.save_map(result, test);
}

template<typename ValueMV>
void GeneralKbelik<ValueMV>::build_bsds(istream& jsons, ByteSerializerDeserializers& bsds) {
  auto huff = HuffmanTree();

  auto remember_pos = jsons.tellg();

  string line;
  while (getline(jsons, line)) {
    huff.add(line);
  }

  huff.build();

  bsds.huffman = huff;

  jsons.clear();
  jsons.seekg(remember_pos);
}

template<typename ValueMV>
void GeneralKbelik<ValueMV>::build_map(istream& jsons, DynamicMap<map_values::ID, ValueMV>& dm) {
  string line;
  while (getline(jsons, line)) {
    auto js = Json::parse(line);
    string q_str = js["qid"];
    auto id = ID(q_str);
    auto aei = typename ValueMV::Type(js);
    dm.add(id, aei);
  }
}

template<typename ValueMV>
GeneralKbelik<ValueMV>::GeneralKbelik(filesystem::path kbelik_path, size_t offset, int64_t /*length*/) {
  this->kbelik_path = kbelik_path;
  size_t bsds_bytes = load_bsds(offset);
  load_map(offset + bsds_bytes, -1);
}

template<typename ValueMV>
bool GeneralKbelik<ValueMV>::find(ID id, typename ValueMV::Type& value) const {
  return map->find(id, value);
}

template<typename ValueMV>
void GeneralKbelik<ValueMV>::close() {
  map->close();
}

template<typename ValueMV>
bool GeneralKbelik<ValueMV>::opened() const {
  return map->opened();
}

template<typename ValueMV>
size_t GeneralKbelik<ValueMV>::load_bsds(size_t offset) {
  size_t bsds_size = 0;

  std::ifstream ifs(kbelik_path, std::ios::binary | std::ios::in);
  byte b;

  auto huff = HuffmanTree();
  vector<byte> huff_bytes;


  if (ifs.is_open()) {
    ifs.seekg(offset);
    while (ifs.read((char*)&b, sizeof(b))) {
      huff_bytes.push_back(b);
      if(b == huff.end_serialize_symbol()) 
        break;
    }
    ifs.close();
  } 
  else {
    throw LinpipeError("Failed to open file for reading.\n");
  }
  huff.deserialize(huff_bytes);
  bsds.huffman = huff;

  bsds_size += huff_bytes.size();
  return bsds_size;
}

template<typename ValueMV>
void GeneralKbelik<ValueMV>::load_map(size_t offset, int64_t length) {
  map = new PersistentMap<map_values::ID, ValueMV>(kbelik_path, offset, length, &bsds);
}

template<typename ValueMV>
GeneralKbelik<ValueMV>::~GeneralKbelik() {
  close();
  delete map;
}


} // namespace linpipe::kbelik
