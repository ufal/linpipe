#include "common.h"
#include "dev/kbelik/map_value.h"
#include "dev/kbelik/byte_value.h"
#include <iterator>
#include <cstring>

namespace linpipe {
namespace kbelik {
vector<byte> ByteValue::serialize() {
  vector<byte> out;
  out.push_back(this->data_size());
  //copy(data.begin(), data.end(), back_inserter(out));
  for (int i = 0; i < (int)out[0]; ++i)
    out.push_back(data[i]);
  return out;
}

void ByteValue::deserialize(unique_ptr<byte[]> data_in) {
  size_t size = (size_t)data_in[0];
  data.resize(size);
  std::memcpy(data.data(), &data_in[1], size);
}

ByteValue::ByteValue(unique_ptr<byte[]> data_in) {
  deserialize(move(data_in));
}

byte ByteValue::data_size() {
  if (data.size() > 255) 
    LinpipeError("ByteValue is for testing only and therefore doesn't implement support for more than 255 bytes.");
  cout << data.size() << "data size\n";
  return (byte) data.size();
}

}
}
