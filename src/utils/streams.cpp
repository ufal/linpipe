// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "lib/xz_stream.h"
#include "utils/streams.h"

namespace linpipe {

namespace {

// A streambuf passing the data directly to a target streambuf; its purpose is to throw LinPipeError on failure.
class CheckedOStreamBuf : public std::streambuf {
 public:
  CheckedOStreamBuf(std::streambuf* target, std::string description, std::source_location location)
      : target_(target), description_(std::move(description)), location_(location) {
    if (!target_)
      throw LinPipeError(location_, "Cannot write to {}, it has no stream buffer", description_);
  }

 protected:
  int_type overflow(int_type c) override {
    if (!traits_type::eq_int_type(c, traits_type::eof()) &&
        traits_type::eq_int_type(target_->sputc(traits_type::to_char_type(c)), traits_type::eof()))
      throw LinPipeError(location_, "Cannot write to {}", description_);
    return traits_type::not_eof(c);
  }

  std::streamsize xsputn(const char* data, std::streamsize size) override {
    if (target_->sputn(data, size) != size)
      throw LinPipeError(location_, "Cannot write to {}", description_);
    return size;
  }

  int sync() override {
    if (target_->pubsync() == -1)
      throw LinPipeError(location_, "Cannot flush {}", description_);
    return 0;
  }

 private:
  std::streambuf* target_;
  std::string description_;
  std::source_location location_;
};

} // namespace

// Compression
Compression compression_from_name(std::string_view name, bool allow_auto, std::source_location location) {
  if (name == "auto" && allow_auto) return Compression::AUTO;
  if (name.empty() || name == "none") return Compression::NONE;
  if (name == "xz") return Compression::XZ;
  throw LinPipeError(location, "Unsupported compression '{}'", name);
}

std::string_view compression_name(Compression compression, std::source_location location) {
  switch (compression) {
    case Compression::AUTO:
      return "auto";
    case Compression::NONE:
      return "none";
    case Compression::XZ:
      return "xz";
  }
  throw LinPipeError(location, "Unsupported compression value '{}'", static_cast<int>(compression));
}

Compression compression_from_path(const std::filesystem::path& path) {
  return path.extension() == ".xz" ? Compression::XZ : Compression::NONE;
}

// InputStream
class InputStream::Codec {
 public:
  virtual ~Codec() = default;
  virtual std::streambuf* streambuf() = 0;

  static std::unique_ptr<Codec> create(std::istream& source, Compression compression, std::source_location location);

 private:
  class XZ;
};

class InputStream::Codec::XZ : public InputStream::Codec {
 public:
  XZ(std::istream& source, std::source_location location) : buf_(source, location) {}
  std::streambuf* streambuf() override { return &buf_; }

 private:
  XZIStreamBuf buf_;
};

std::unique_ptr<InputStream::Codec> InputStream::Codec::create(std::istream& source, Compression compression, std::source_location location) {
  switch (compression) {
    case Compression::XZ:
      return std::make_unique<XZ>(source, location);
    default:
      throw LinPipeError(location, "Unsupported compression '{}' for input stream", compression_name(compression));
  }
}

InputStream::InputStream(std::istream& source, Compression compression, std::source_location location)
    : std::istream(nullptr), location_(location) {
  init(source, compression);
}

InputStream::InputStream(const std::filesystem::path& path, std::ios::openmode openmode, Compression compression, std::source_location location)
    : std::istream(nullptr), location_(location), path_(path) {
  if (compression == Compression::AUTO)
    compression = compression_from_path(path);
  if (compression != Compression::NONE)
    openmode |= std::ios::binary;

  file_ = std::make_unique<std::ifstream>(path, openmode | std::ios::in);
  if (!file_->is_open())
    throw LinPipeError(location_, "Cannot open file {} for reading", path);

  init(*file_, compression);
}

InputStream::~InputStream() {
  try {
    close(location_);
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
}

void InputStream::init(std::istream& source, Compression compression) {
  if (compression == Compression::AUTO)
    compression = Compression::NONE;

  if (compression == Compression::NONE) {
    rdbuf(source.rdbuf());
  } else {
    codec_ = Codec::create(source, compression, location_);
    rdbuf(codec_->streambuf());
  }
  exceptions(std::ios::badbit);
}

void InputStream::close(std::source_location location) {
  if (closed_)
    return;
  closed_ = true;

  if (file_) {
    file_->clear();  // the state can contain eofbit/failbit, e.g., after reading the whole file
    file_->close();
    if (file_->fail())
      throw LinPipeError(location, "Cannot close file {}", path_);
  }
}

// OutputStream
class OutputStream::Codec {
 public:
  virtual ~Codec() = default;
  virtual std::streambuf* streambuf() = 0;
  virtual void finish() = 0;

  static std::unique_ptr<Codec> create(std::ostream& target, Compression compression, CompressionMode mode, std::string description,
                                       std::source_location location);

 private:
  class None;
  class XZ;
};

class OutputStream::Codec::None : public OutputStream::Codec {
 public:
  None(std::ostream& target, std::string description, std::source_location location)
      : buf_(target.rdbuf(), std::move(description), location) {}
  std::streambuf* streambuf() override { return &buf_; }
  void finish() override { buf_.pubsync(); }

 private:
  CheckedOStreamBuf buf_;
};

class OutputStream::Codec::XZ : public OutputStream::Codec {
 public:
  XZ(std::ostream& target, CompressionMode mode, std::source_location location)
      : buf_(target, mode == CompressionMode::COMPRESS ? XZMode::COMPRESS : XZMode::DECOMPRESS, location) {}
  std::streambuf* streambuf() override { return &buf_; }
  void finish() override { buf_.finish(); }

 private:
  XZOStreamBuf buf_;
};

std::unique_ptr<OutputStream::Codec> OutputStream::Codec::create(std::ostream& target, Compression compression, CompressionMode mode,
                                                                 std::string description, std::source_location location) {
  switch (compression) {
    case Compression::NONE:
      return std::make_unique<None>(target, std::move(description), location);
    case Compression::XZ:
      return std::make_unique<XZ>(target, mode, location);
    default:
      throw LinPipeError(location, "Unsupported compression '{}' for output stream", compression_name(compression));
  }
}

OutputStream::OutputStream(std::ostream& target, Compression compression, CompressionMode mode, std::source_location location)
    : std::ostream(nullptr), location_(location) {
  init(target, compression, mode);
}

OutputStream::OutputStream(const std::filesystem::path& path, std::ios::openmode openmode, Compression compression, CompressionMode mode, std::source_location location)
    : std::ostream(nullptr), location_(location), path_(path) {
  if (compression == Compression::AUTO) {
    if (mode == CompressionMode::DECOMPRESS)
      throw LinPipeError(location_, "Cannot use automatic compression detection when decompressing to file {}", path);
    compression = compression_from_path(path);
  }
  if (compression != Compression::NONE)
    openmode |= std::ios::binary;

  file_ = std::make_unique<std::ofstream>(path, openmode | std::ios::out);
  if (!file_->is_open())
    throw LinPipeError(location_, "Cannot open file {} for writing", path);

  init(*file_, compression, mode);
}

OutputStream::~OutputStream() {
  try {
    close(location_);
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
}

void OutputStream::init(std::ostream& target, Compression compression, CompressionMode mode) {
  if (compression == Compression::AUTO)
    compression = Compression::NONE;

  codec_ = Codec::create(target, compression, mode, file_ ? fmt::format("file {}", path_) : "the output stream", location_);
  rdbuf(codec_->streambuf());
  exceptions(std::ios::badbit);
}

void OutputStream::close(std::source_location location) {
  if (closed_)
    return;
  closed_ = true;

  // A bad stream has already thrown; do not finish a possibly incomplete output.
  if (!bad())
    codec_->finish();

  if (file_) {
    file_->close();
    if (file_->fail())
      throw LinPipeError(location, "Cannot close file {}", path_);
  }
}

// MultiInputStream
//
// The StreamBuf has no buffer of its own and passes the characters one by one
// from the current InputStream. The unit boundaries are detected by peeking
// the next character.
class MultiInputStream::StreamBuf : public std::streambuf {
 public:
  StreamBuf(std::istream& source, Compression compression, std::optional<StreamLimit> limit, std::source_location location)
      : limit_(limit), location_(location), input_(std::make_unique<InputStream>(source, compression, location)), in_(input_->rdbuf()) {}

  StreamBuf(std::vector<std::filesystem::path> paths, std::ios::openmode openmode, Compression compression,
            std::optional<StreamLimit> limit, std::source_location location)
      : paths_(std::move(paths)), openmode_(openmode), compression_(compression), limit_(limit), location_(location) {}

  bool next_batch() {
    if (!limit_)
      return false;

    batch_done_ = false;
    batch_units_ = batch_bytes_ = 0;
    underflow();
    return input_ != nullptr;
  }

  std::optional<std::filesystem::path> path() {
    underflow();
    return input_ ? input_->path() : std::nullopt;
  }

  void close(std::source_location location) {
    next_path_ = paths_.size();
    if (input_) {
      auto input = std::move(input_);
      in_ = nullptr;
      input->close(location);
    }
  }

 protected:
  int_type underflow() override {
    // Advance to the next non-empty input, if the current one is exhausted.
    while (!input_ || traits_type::eq_int_type(in_->sgetc(), traits_type::eof())) {
      if (input_) end_unit();
      if (!open_next())
        return traits_type::eof();
    }
    if (batch_done_)
      return traits_type::eof();

    int_type c = in_->sgetc();
    if (limit_ && limit_->unit != StreamLimit::Unit::FILE && line_start_ && unit_has_content_ && c != '\n' && c != '\r' &&
        (limit_->unit == StreamLimit::Unit::LINE || seen_empty_)) {
      end_unit();
      if (batch_done_)
        return traits_type::eof();
    }
    return c;
  }

  int_type uflow() override {
    int_type c = underflow();
    if (traits_type::eq_int_type(c, traits_type::eof()))
      return c;
    in_->sbumpc();
    offset_++;
    batch_bytes_++;

    if (c == '\n' && prev_cr_) {
      prev_cr_ = false;  // the second character of a "\r\n" line end
    } else if (c == '\n' || c == '\r') {
      if (line_start_) seen_empty_ = true;
      line_start_ = true;
      prev_cr_ = c == '\r';
    } else {
      if (line_start_) {
        unit_has_content_ = true;
        seen_empty_ = false;
      }
      line_start_ = prev_cr_ = false;
    }
    if (limit_ && limit_->unit == StreamLimit::Unit::FILE)
      unit_has_content_ = true;
    return c;
  }

  pos_type seekoff(off_type off, std::ios::seekdir dir, std::ios::openmode which) override {
    if (off != 0 || dir != std::ios::cur || !(which & std::ios::in))
      return {off_type{-1}};
    underflow();
    return {input_ ? static_cast<off_type>(offset_) : off_type{-1}};
  }

 private:
  void end_unit() {
    if (!unit_has_content_)
      return;
    unit_has_content_ = false;
    if (!limit_)
      return;
    batch_units_++;
    if ((limit_->kind == StreamLimit::Kind::UNITS ? batch_units_ : batch_bytes_) >= limit_->value)
      batch_done_ = true;
  }

  // Close the current input and open the next one, returning false if there is none.
  bool open_next() {
    if (input_) {
      auto input = std::move(input_);
      in_ = nullptr;
      input->close(location_);
    }
    if (next_path_ < paths_.size()) {
      input_ = std::make_unique<InputStream>(paths_[next_path_++], openmode_, compression_, location_);
      in_ = input_->rdbuf();
      offset_ = 0;
      line_start_ = true;
      prev_cr_ = seen_empty_ = false;
    }
    return input_ != nullptr;
  }

  std::vector<std::filesystem::path> paths_;
  size_t next_path_ = 0;
  std::ios::openmode openmode_ = std::ios::binary;
  Compression compression_ = Compression::NONE;
  std::optional<StreamLimit> limit_;
  std::source_location location_;

  std::unique_ptr<InputStream> input_;
  std::streambuf* in_ = nullptr;
  uint64_t offset_ = 0;

  bool line_start_ = true, prev_cr_ = false, seen_empty_ = false, unit_has_content_ = false;
  size_t batch_units_ = 0, batch_bytes_ = 0;
  bool batch_done_ = false;
};

MultiInputStream::MultiInputStream(std::istream& source, Compression compression, std::optional<StreamLimit> limit, std::source_location location)
    : std::istream(nullptr), location_(location), buf_(std::make_unique<StreamBuf>(source, compression, limit, location)) {
  rdbuf(buf_.get());
  exceptions(std::ios::badbit);
}

MultiInputStream::MultiInputStream(std::vector<std::filesystem::path> paths, std::ios::openmode openmode, Compression compression,
                                   std::optional<StreamLimit> limit, std::source_location location)
    : std::istream(nullptr), location_(location), buf_(std::make_unique<StreamBuf>(std::move(paths), openmode, compression, limit, location)) {
  rdbuf(buf_.get());
  exceptions(std::ios::badbit);
}

MultiInputStream::~MultiInputStream() {
  try {
    close(location_);
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
}

bool MultiInputStream::next_batch() {
  clear();
  return buf_->next_batch();
}

std::optional<std::filesystem::path> MultiInputStream::path() const {
  return buf_->path();
}

void MultiInputStream::close(std::source_location location) {
  if (closed_)
    return;
  closed_ = true;

  buf_->close(location);
}

} // namespace linpipe
