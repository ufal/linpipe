#pragma once

#include "common.h"

#include "dev/kbelik/huffman.h"

namespace linpipe::kbelik {

struct ByteSerializerDeserializers {
  ByteSerializerDeserializers() {
    huffman = HuffmanTree();
  }
  HuffmanTree huffman;
};

} // linpipe::kbelik
