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

std::string describe_stream(const std::filesystem::path& path, std::string_view stream) {
  return path.empty() ? std::string{stream} : fmt::format("file {}", path);
}

} // namespace

// Compression
Compression compression_from_name(std::string_view name, bool allow_auto, std::source_location location) {
  if (name == "auto" && allow_auto) return Compression::AUTO;
  if (name.empty() || name == "none") return Compression::NONE;
  if (name == "xz") return Compression::XZ;
  throw LinpipeError(location, "Unsupported compression '{}'", name);
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
  throw LinpipeError(location, "Unsupported compression value '{}'", static_cast<int>(compression));
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
      throw LinpipeError(location, "Unsupported compression '{}' for input stream", compression_name(compression));
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
    throw LinpipeError(location_, "Cannot open file {} for reading", path);

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
      throw LinpipeError(location, "Cannot close file {}", path_);
  }
  if (bad())
    throw LinpipeError(location, "Cannot close {}, it is in a failed state", describe_stream(path_, "the input stream"));
}

// OutputStream
class OutputStream::Codec {
 public:
  virtual ~Codec() = default;
  virtual std::streambuf* streambuf() = 0;
  virtual void finish() = 0;

  static std::unique_ptr<Codec> create(std::ostream& target, Compression compression, CompressionMode mode, std::source_location location);

 private:
  class XZ;
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

std::unique_ptr<OutputStream::Codec> OutputStream::Codec::create(std::ostream& target, Compression compression, CompressionMode mode, std::source_location location) {
  switch (compression) {
    case Compression::XZ:
      return std::make_unique<XZ>(target, mode, location);
    default:
      throw LinpipeError(location, "Unsupported compression '{}' for output stream", compression_name(compression));
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
      throw LinpipeError(location_, "Cannot use automatic compression detection when decompressing to file {}", path);
    compression = compression_from_path(path);
  }
  if (compression != Compression::NONE)
    openmode |= std::ios::binary;

  file_ = std::make_unique<std::ofstream>(path, openmode | std::ios::out);
  if (!file_->is_open())
    throw LinpipeError(location_, "Cannot open file {} for writing", path);

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

  if (compression == Compression::NONE) {
    rdbuf(target.rdbuf());
  } else {
    codec_ = Codec::create(target, compression, mode, location_);
    rdbuf(codec_->streambuf());
  }
  exceptions(std::ios::badbit);
}

void OutputStream::close(std::source_location location) {
  if (closed_)
    return;
  closed_ = true;

  if (bad())
    throw LinpipeError(location, "Cannot close {}, it is in a failed state", describe_stream(path_, "the output stream"));
  if (codec_)
    codec_->finish();
  else
    flush();

  if (file_) {
    file_->close();
    if (file_->fail())
      throw LinpipeError(location, "Cannot close file {}", path_);
  }
}

} // namespace linpipe
