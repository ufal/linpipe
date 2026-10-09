// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "lib/liblzma/api/lzma.h"
#include "lib/xz.h"

namespace linpipe {

namespace {

template<typename C>
bool compress(std::span<const typename C::value_type> data, C& output, uint32_t preset) {
  output.clear();

  lzma_stream stream = LZMA_STREAM_INIT;
  lzma_ret ret = lzma_easy_encoder(&stream, preset, LZMA_CHECK_CRC64);
  if (ret != LZMA_OK)
    return false;

  output.resize(std::max(data.size() / 4, 128_uz));

  stream.next_in = reinterpret_cast<const uint8_t*>(data.data());
  stream.avail_in = data.size();
  stream.next_out = reinterpret_cast<uint8_t*>(output.data());
  stream.avail_out = output.size();

  while (ret = lzma_code(&stream, stream.avail_in ? LZMA_RUN : LZMA_FINISH), ret != LZMA_STREAM_END) {
    if (ret != LZMA_OK) {
      lzma_end(&stream);
      return false;
    }

    if (!stream.avail_out) {
      size_t current = output.size();
      output.resize(2 * current);
      stream.next_out = reinterpret_cast<uint8_t*>(output.data()) + current;
      stream.avail_out = current;
    }
  }
  output.resize(stream.next_out - reinterpret_cast<const uint8_t*>(output.data()));

  lzma_end(&stream);
  return true;
}

template<typename C>
size_t decompress(std::span<const typename C::value_type> data, C& output, bool only_first_block) {
  output.clear();

  lzma_stream stream = LZMA_STREAM_INIT;
  lzma_ret ret = lzma_stream_decoder(&stream, UINT64_MAX, only_first_block ? 0 : LZMA_CONCATENATED);
  if (ret != LZMA_OK)
    return 0;

  output.resize(std::max(data.size(), 128_uz));

  stream.next_in = reinterpret_cast<const uint8_t*>(data.data());
  stream.avail_in = data.size();
  stream.next_out = reinterpret_cast<uint8_t*>(output.data());
  stream.avail_out = output.size();

  while (ret = lzma_code(&stream, stream.avail_in ? LZMA_RUN : LZMA_FINISH), ret != LZMA_STREAM_END) {
    if (ret != LZMA_OK) {
      lzma_end(&stream);
      return 0;
    }

    if (!stream.avail_out) {
      size_t current = output.size();
      output.resize(2 * current);
      stream.next_out = reinterpret_cast<uint8_t*>(output.data()) + current;
      stream.avail_out = current;
    }
  }
  output.resize(stream.next_out - reinterpret_cast<const uint8_t*>(output.data()));

  lzma_end(&stream);
  return stream.next_in - reinterpret_cast<const uint8_t*>(data.data());
}

} // namespace

bool xz_compress(std::string_view data, std::string& output, uint32_t preset) {
  return compress(data, output, preset);
}

bool xz_compress(std::span<std::byte> data, std::vector<std::byte>& output, uint32_t preset) {
  return compress(data, output, preset);
}

size_t xz_decompress(std::string_view data, std::string& output, bool only_first_block) {
  return decompress(data, output, only_first_block);
}

size_t xz_decompress(std::span<std::byte> data, std::vector<std::byte>& output, bool only_first_block) {
  return decompress(data, output, only_first_block);
}

} // namespace linpipe
