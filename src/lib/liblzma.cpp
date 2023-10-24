// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "lib/liblzma.h"
#include "lib/liblzma/api/lzma.h"

namespace linpipe {

bool lzma_compress(const byte* data, size_t length, vector<byte>& output, uint32_t preset) {
  output.clear();

  lzma_stream stream = LZMA_STREAM_INIT;
  lzma_ret ret = lzma_easy_encoder(&stream, preset, LZMA_CHECK_CRC64);
  if (ret != LZMA_OK)
    return false;

  output.resize(max(length / 4, size_t(128)));

  stream.next_in = (const uint8_t*)data;
  stream.avail_in = length;
  stream.next_out = (uint8_t*)output.data();
  stream.avail_out = output.size();

  while (ret = lzma_code(&stream, stream.avail_in ? LZMA_RUN : LZMA_FINISH), ret != LZMA_STREAM_END) {
    if (ret != LZMA_OK) {
      lzma_end(&stream);
      return false;
    }

    if (!stream.avail_out) {
      size_t current = output.size();
      output.resize(2 * current);
      stream.next_out = (uint8_t*)output.data() + current;
      stream.avail_out = current;
    }
  }
  output.resize(stream.next_out - (const uint8_t*)output.data());

  lzma_end(&stream);
  return true;
}

bool lzma_decompress_all(const byte* data, size_t length, vector<byte>& output) {
  output.clear();

  lzma_stream stream = LZMA_STREAM_INIT;
  lzma_ret ret = lzma_stream_decoder(&stream, UINT64_MAX, LZMA_CONCATENATED);
  if (ret != LZMA_OK)
    return false;

  output.resize(max(length, size_t(128)));

  stream.next_in = (const uint8_t*)data;
  stream.avail_in = length;
  stream.next_out = (uint8_t*)output.data();
  stream.avail_out = output.size();

  while (ret = lzma_code(&stream, stream.avail_in ? LZMA_RUN : LZMA_FINISH), ret != LZMA_STREAM_END) {
    if (ret != LZMA_OK) {
      lzma_end(&stream);
      return false;
    }

    if (!stream.avail_out) {
      size_t current = output.size();
      output.resize(2 * current);
      stream.next_out = (uint8_t*)output.data() + current;
      stream.avail_out = current;
    }
  }
  output.resize(stream.next_out - (const uint8_t*)output.data());

  lzma_end(&stream);
  return true;
}

size_t lzma_decompress_one(const byte* data, size_t length, vector<byte>& output) {
  output.clear();

  lzma_stream stream = LZMA_STREAM_INIT;
  lzma_ret ret = lzma_stream_decoder(&stream, UINT64_MAX, 0);
  if (ret != LZMA_OK)
    return 0;

  output.resize(max(length, size_t(128)));

  stream.next_in = (const uint8_t*)data;
  stream.avail_in = length;
  stream.next_out = (uint8_t*)output.data();
  stream.avail_out = output.size();

  while (ret = lzma_code(&stream, stream.avail_in ? LZMA_RUN : LZMA_FINISH), ret != LZMA_STREAM_END) {
    if (ret != LZMA_OK) {
      lzma_end(&stream);
      return 0;
    }

    if (!stream.avail_out) {
      size_t current = output.size();
      output.resize(2 * current);
      stream.next_out = (uint8_t*)output.data() + current;
      stream.avail_out = current;
    }
  }
  output.resize(stream.next_out - (const uint8_t*)output.data());

  lzma_end(&stream);
  return (const byte*)stream.next_in - data;
}

} // namespace linpipe
