// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <fstream>

#include "common.h"

namespace linpipe {

// Compression
enum class Compression {
  AUTO,  // from the path suffix (".xz" -> XZ, otherwise NONE); NONE for streams
  NONE,
  XZ,
};

enum class CompressionMode {
  COMPRESS,
  DECOMPRESS,
};

Compression compression_from_name(std::string_view name, bool allow_auto = true, std::source_location location = std::source_location::current());
Compression compression_from_path(const std::filesystem::path& path);
std::string_view compression_name(Compression compression, std::source_location location = std::source_location::current());

// Input and output streams with optional (de)compression, either wrapping
// a non-owned stream, or owning a file opened from a path.
class InputStream : public std::istream {
 public:
  explicit InputStream(std::istream& source, Compression compression = Compression::NONE,
                       std::source_location location = std::source_location::current());

  explicit InputStream(const std::filesystem::path& path, std::ios::openmode openmode = std::ios::binary,
                       Compression compression = Compression::AUTO,
                       std::source_location location = std::source_location::current());

  ~InputStream() override;

  InputStream(const InputStream&) = delete;
  InputStream& operator=(const InputStream&) = delete;

  // Close the decompressor and possibly the owned file.
  void close(std::source_location location = std::source_location::current());

  // An optional path to the owned file.
  std::optional<std::filesystem::path> path() const { return file_ ? std::optional{path_} : std::nullopt; }

 private:
  class Codec;
  void init(std::istream& source, Compression compression);

  std::source_location location_;
  std::filesystem::path path_;
  std::unique_ptr<std::ifstream> file_;  // must be destroyed after codec_
  std::unique_ptr<Codec> codec_;
  bool closed_ = false;
};

class OutputStream : public std::ostream {
 public:
  explicit OutputStream(std::ostream& target, Compression compression = Compression::NONE,
                        CompressionMode mode = CompressionMode::COMPRESS,
                        std::source_location location = std::source_location::current());

  explicit OutputStream(const std::filesystem::path& path, std::ios::openmode openmode = std::ios::binary,
                        Compression compression = Compression::AUTO,
                        CompressionMode mode = CompressionMode::COMPRESS,
                        std::source_location location = std::source_location::current());

  ~OutputStream() override;

  OutputStream(const OutputStream&) = delete;
  OutputStream& operator=(const OutputStream&) = delete;

  // Finish the (de)compression, flush, and possibly close the owned file.
  void close(std::source_location location = std::source_location::current());

  // An optional path to the owned file.
  std::optional<std::filesystem::path> path() const { return file_ ? std::optional{path_} : std::nullopt; }

 private:
  class Codec;
  void init(std::ostream& target, Compression compression, CompressionMode mode);

  std::source_location location_;
  std::filesystem::path path_;
  std::unique_ptr<std::ofstream> file_;  // must be destroyed after codec_
  std::unique_ptr<Codec> codec_;
  bool closed_ = false;
};

} // namespace linpipe
