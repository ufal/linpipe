#pragma once

#include <filesystem>
#include <type_traits>

#include "lib/json.h"

#include "common.h"

#include "dev/kbelik/byte_serializer_deserializer.h"
#include "dev/kbelik/dynamic_map.h"
#include "dev/kbelik/id.h"
#include "dev/kbelik/named_entity_mapper.h"
#include "dev/kbelik/persistent_map.h"
#include "dev/kbelik/utils.h"

#include "dev/kbelik/map_values/agnostic_entity_info_huff.h"
#include "dev/kbelik/map_keys/qid8.h"
#include "dev/kbelik/map_values/id.h"
#include "dev/kbelik/map_values/specific_entity_info_huff.h"

namespace linpipe::kbelik {

template<typename MapKey, typename ValueMV>
class GeneralKbelik {
 public:
  // Rovnou map_path
  //GeneralKbelik(filesystem::path map_path, size_t offset=0, int64_t length=-1);
  ~GeneralKbelik();

  bool find(ID id, typename ValueMV::Type& value) const { return map->find(id, value); }
  void close() { map->close(); }

  bool opened() const { return map->close(); }
 protected:
  filesystem::path kbelik_path;
  HuffmanTree huffman;
  PersistentMap<MapKey, ValueMV>* map = nullptr;

  size_t load_huffman (size_t offset);

  virtual void load_map (size_t offset, int64_t length) = 0;

  static inline void build_huffman(istream& jsons, HuffmanTree& huffman);
};


template<typename MapKey, typename ValueMV>
void GeneralKbelik<MapKey, ValueMV>::build_huffman(istream& jsons, HuffmanTree& huff) {
  auto remember_pos = jsons.tellg();

  string line;
  while (getline(jsons, line)) {
    huff.add(line);
  }

  huff.build();

  jsons.clear();
  jsons.seekg(remember_pos);
}

template<typename MapKey, typename ValueMV>
size_t GeneralKbelik<MapKey, ValueMV>::load_huffman(size_t offset) {
  size_t huffman_size = 0;

  std::ifstream ifs(kbelik_path, std::ios::binary | std::ios::in);
  byte b;

  huffman = HuffmanTree();
  vector<byte> huff_bytes;


  if (ifs.is_open()) {
    ifs.seekg(offset);
    while (ifs.read((char*)&b, sizeof(b))) {
      huff_bytes.push_back(b);
      if(b == huffman.end_serialize_symbol()) 
        break;
    }
    ifs.close();
  } 
  else {
    throw LinpipeError("Failed to open file for reading.\n");
  }
  huffman.deserialize(huff_bytes);

  huffman_size = huff_bytes.size();
  return huffman_size;
}

template<typename MapKey, typename ValueMV>
GeneralKbelik<MapKey, ValueMV>::~GeneralKbelik() {
  close();
  delete map;
}

} // namespace linpipe::kbelik
