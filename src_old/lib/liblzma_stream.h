// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <istream>
#include <ostream>
#include <streambuf>

#include "common.h"

namespace linpipe::lzma {

enum class Mode {
  COMPRESS,
  DECOMPRESS,
};

struct Coder;

class OStreamBuf : public std::streambuf {
 public:
  OStreamBuf(std::ostream& target, Mode mode, uint32_t preset);
  ~OStreamBuf() override;

  bool finish();

 protected:
  int_type overflow(int_type c) override;
  int sync() override;

 private:
  bool code(bool finish);

  std::ostream& target_;
  std::unique_ptr<Coder> coder_;
  std::vector<char> input_, output_;
  bool finished_ = false;
};

class IStreamBuf : public std::streambuf {
 public:
  IStreamBuf(std::istream& source);
  ~IStreamBuf() override;

 protected:
  int_type underflow() override;

 private:
  std::istream& source_;
  std::unique_ptr<Coder> coder_;
  std::vector<char> input_, output_;
  bool source_finished_ = false, finished_ = false;
};

class OStream : public std::ostream {
 public:
  OStream(std::ostream& target, Mode mode, uint32_t preset = 6);

  void close();

 private:
  OStreamBuf buf_;
};

class IStream : public std::istream {
 public:
  IStream(std::istream& source);

 private:
  IStreamBuf buf_;
};

} // namespace linpipe::lzma
