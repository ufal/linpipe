// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/liblzma/api/lzma.h"
#include "lib/liblzma_stream.h"

namespace linpipe::lzma {

constexpr size_t BUFFER_SIZE = 4 << 10;  // 4kB

struct Coder {
  lzma_stream stream = LZMA_STREAM_INIT;
  ~Coder() { lzma_end(&stream); }
};

// OStreamBuf
OStreamBuf::OStreamBuf(std::ostream& target, Mode mode, uint32_t preset)
    : target_(target), coder_(std::make_unique<Coder>()), input_(BUFFER_SIZE), output_(BUFFER_SIZE) {
  lzma_ret ret;
  if (mode == Mode::COMPRESS)
    ret = lzma_easy_encoder(&coder_->stream, preset, LZMA_CHECK_CRC64);
  else
    ret = lzma_stream_decoder(&coder_->stream, UINT64_MAX, LZMA_CONCATENATED);

  if (ret != LZMA_OK)
    throw LinpipeError{"lzma::OStreamBuf: Cannot initialize the LZMA ", mode == Mode::COMPRESS ? "encoder" : "decoder"};

  setp(input_.data(), input_.data() + input_.size());
}

OStreamBuf::~OStreamBuf() {
  try {
    finish();
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
}

bool OStreamBuf::finish() {
  if (finished_)
    return true;
  bool ok = code(true) && target_.flush();
  finished_ = true;
  return ok;
}

OStreamBuf::int_type OStreamBuf::overflow(int_type c) {
  if (finished_ || !code(false))
    return traits_type::eof();

  if (!traits_type::eq_int_type(c, traits_type::eof())) {
    *pptr() = traits_type::to_char_type(c);
    pbump(1);
  }
  return traits_type::not_eof(c);
}

int OStreamBuf::sync() {
  return !finished_ && code(false) && target_.flush() ? 0 : -1;
}

bool OStreamBuf::code(bool finish) {
  lzma_stream& stream = coder_->stream;
  stream.next_in = (const uint8_t*)pbase();
  stream.avail_in = pptr() - pbase();

  lzma_ret ret;
  do {
    stream.next_out = (uint8_t*)output_.data();
    stream.avail_out = output_.size();
    ret = lzma_code(&stream, finish ? LZMA_FINISH : LZMA_RUN);
    if (ret != LZMA_OK && ret != LZMA_STREAM_END)
      throw LinpipeError{"lzma::OStreamBuf: An error occurred during LZMA coding"};
    if (!target_.write(output_.data(), (std::streamsize)(output_.size() - stream.avail_out)))
      return false;
  } while (ret != LZMA_STREAM_END && (finish || stream.avail_in || !stream.avail_out));

  setp(input_.data(), input_.data() + input_.size());
  return true;
}

// IStreamBuf
IStreamBuf::IStreamBuf(std::istream& source)
    : source_(source), coder_(std::make_unique<Coder>()), input_(BUFFER_SIZE), output_(BUFFER_SIZE) {
  if (lzma_stream_decoder(&coder_->stream, UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK)
    throw LinpipeError{"lzma::IStreamBuf: Cannot initialize the LZMA decoder"};
}

IStreamBuf::~IStreamBuf() = default;

IStreamBuf::int_type IStreamBuf::underflow() {
  if (gptr() < egptr())
    return traits_type::to_int_type(*gptr());
  if (finished_)
    return traits_type::eof();

  lzma_stream& stream = coder_->stream;
  stream.next_out = (uint8_t*)output_.data();
  stream.avail_out = output_.size();

  while (stream.avail_out == output_.size()) {
    if (!stream.avail_in && !source_finished_) {
      source_.read(input_.data(), (std::streamsize)input_.size());
      stream.next_in = (const uint8_t*)input_.data();
      stream.avail_in = source_.gcount();
      source_finished_ = !source_;
    }

    lzma_ret ret = lzma_code(&stream, source_finished_ ? LZMA_FINISH : LZMA_RUN);
    if (ret == LZMA_STREAM_END) {
      finished_ = true;
      break;
    }
    if (ret != LZMA_OK)
      throw LinpipeError{"lzma::IStreamBuf: An error occurred during LZMA decoding"};
  }

  setg(output_.data(), output_.data(), (char*)stream.next_out);
  return gptr() < egptr() ? traits_type::to_int_type(*gptr()) : traits_type::eof();
}

// OStream
OStream::OStream(std::ostream& target, Mode mode, uint32_t preset)
    : std::ostream(nullptr), buf_(target, mode, preset) {
  rdbuf(&buf_);
}

void OStream::close() {
  bool ok = false;
  try {
    ok = buf_.finish();
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
  if (!ok)
    setstate(std::ios::badbit);
}

// IStream
IStream::IStream(std::istream& source)
    : std::istream(nullptr), buf_(source) {
  rdbuf(&buf_);
}

} // namespace linpipe::lzma
