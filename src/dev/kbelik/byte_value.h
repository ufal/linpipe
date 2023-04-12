#pragma once

#include "common.h"
#include "dev/kbelik/map_value.h"

namespace linpipe {
namespace kbelik {
  class ByteValue : public MapValue { 
    public:
      ByteValue(unique_ptr<byte[]> data_in);
      void deserialize(unique_ptr<byte[]> data_in) override;
      vector<byte> serialize() override;
    private:
      vector<byte> data;
      byte data_size();
  };
}
}
