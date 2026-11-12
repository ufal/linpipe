// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/liblzma/api/lzma.h"
#include "lib/xz_stream.h"

namespace linpipe {

namespace {

constexpr size_t XZ_BUFFER_SIZE = 4 << 10;  // 4kB
constexpr uint32_t XZ_PRESET = 6;

} // namespace

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class XZCoder {
 public:
  lzma_stream stream = LZMA_STREAM_INIT;
  ~XZCoder() { lzma_end(&stream); }
};

// XZOStreamBuf
XZOStreamBuf::XZOStreamBuf(std::ostream& target, XZMode mode, std::source_location location)
    : target_(target), location_(location), coder_(std::make_unique<XZCoder>()), input_(XZ_BUFFER_SIZE), output_(XZ_BUFFER_SIZE) {
  lzma_ret ret{};
  if (mode == XZMode::COMPRESS)
    ret = lzma_easy_encoder(&coder_->stream, XZ_PRESET, LZMA_CHECK_CRC64);
  else
    ret = lzma_stream_decoder(&coder_->stream, UINT64_MAX, LZMA_CONCATENATED);

  if (ret != LZMA_OK)
    throw LinpipeError(location_, "Cannot initialize the XZ {}", mode == XZMode::COMPRESS ? "encoder" : "decoder");

  setp(input_.data(), input_.data() + input_.size());
}

XZOStreamBuf::~XZOStreamBuf() = default;

void XZOStreamBuf::finish() {
  if (finished_)
    return;
  finished_ = true;

  run_coder(true);
  setp(nullptr, nullptr);
  if (!target_.flush())
    throw LinpipeError(location_, "Cannot flush the XZ stream target");
}

XZOStreamBuf::int_type XZOStreamBuf::overflow(int_type c) {
  if (finished_)
    throw LinpipeError(location_, "Cannot write to an already finished XZ stream");

  run_coder(false);

  if (!traits_type::eq_int_type(c, traits_type::eof())) {
    *pptr() = traits_type::to_char_type(c);
    pbump(1);
  }
  return traits_type::not_eof(c);
}

int XZOStreamBuf::sync() {
  if (finished_)
    return 0;

  run_coder(false);
  if (!target_.flush())
    throw LinpipeError(location_, "Cannot flush the XZ stream target");
  return 0;
}

void XZOStreamBuf::run_coder(bool finish) {
  lzma_stream& stream = coder_->stream;
  stream.next_in = reinterpret_cast<const uint8_t*>(pbase());
  stream.avail_in = pptr() - pbase();

  lzma_ret ret{};
  do {
    stream.next_out = reinterpret_cast<uint8_t*>(output_.data());
    stream.avail_out = output_.size();
    ret = lzma_code(&stream, finish ? LZMA_FINISH : LZMA_RUN);
    if (ret != LZMA_OK && ret != LZMA_STREAM_END)
      throw LinpipeError(location_, "An error occurred during XZ coding");
    if (!target_.write(output_.data(), static_cast<std::streamsize>(output_.size() - stream.avail_out)))
      throw LinpipeError(location_, "Cannot write to the XZ stream target");
  } while (ret != LZMA_STREAM_END && (finish || stream.avail_in || !stream.avail_out));

  setp(input_.data(), input_.data() + input_.size());
}

// XZIStreamBuf
XZIStreamBuf::XZIStreamBuf(std::istream& source, std::source_location location)
    : source_(source), location_(location), coder_(std::make_unique<XZCoder>()), input_(XZ_BUFFER_SIZE), output_(XZ_BUFFER_SIZE) {
  if (lzma_stream_decoder(&coder_->stream, UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK)
    throw LinpipeError(location_, "Cannot initialize the XZ decoder");
}

XZIStreamBuf::~XZIStreamBuf() = default;

XZIStreamBuf::int_type XZIStreamBuf::underflow() {
  if (gptr() < egptr())
    return traits_type::to_int_type(*gptr());
  if (finished_)
    return traits_type::eof();

  lzma_stream& stream = coder_->stream;
  stream.next_out = reinterpret_cast<uint8_t*>(output_.data());
  stream.avail_out = output_.size();

  while (stream.avail_out == output_.size()) {
    if (!stream.avail_in && !source_finished_) {
      source_.read(input_.data(), static_cast<std::streamsize>(input_.size()));
      if (source_.bad())
        throw LinpipeError(location_, "Cannot read from the XZ stream source");
      stream.next_in = reinterpret_cast<const uint8_t*>(input_.data());
      stream.avail_in = source_.gcount();
      source_finished_ = !source_;
    }

    lzma_ret ret = lzma_code(&stream, source_finished_ ? LZMA_FINISH : LZMA_RUN);
    if (ret == LZMA_STREAM_END) {
      finished_ = true;
      break;
    }
    if (ret != LZMA_OK)
      throw LinpipeError(location_, "An error occurred during XZ decoding");
  }

  setg(output_.data(), output_.data(), reinterpret_cast<char*>(stream.next_out));
  return gptr() < egptr() ? traits_type::to_int_type(*gptr()) : traits_type::eof();
}

} // namespace linpipe
