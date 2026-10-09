// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace linpipe {

enum class XZMode {
  COMPRESS,
  DECOMPRESS,
};

class XZCoder;

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class XZOStreamBuf : public std::streambuf {
 public:
  XZOStreamBuf(std::ostream& target, XZMode mode, uint32_t preset);
  ~XZOStreamBuf() override;

  bool finish();

 protected:
  int_type overflow(int_type c) override;
  int sync() override;

 private:
  bool run_coder(bool finish);

  std::ostream& target_;
  std::unique_ptr<XZCoder> coder_;
  std::vector<char> input_, output_;
  bool finished_ = false;
};

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class XZIStreamBuf : public std::streambuf {
 public:
  XZIStreamBuf(std::istream& source);
  ~XZIStreamBuf() override;

 protected:
  int_type underflow() override;

 private:
  std::istream& source_;
  std::unique_ptr<XZCoder> coder_;
  std::vector<char> input_, output_;
  bool source_finished_ = false, finished_ = false;
};

class XZOStream : public std::ostream {
 public:
  XZOStream(std::ostream& target, XZMode mode, uint32_t preset = 6);

  void close();

 private:
  XZOStreamBuf buf_;
};

class XZIStream : public std::istream {
 public:
  XZIStream(std::istream& source);

 private:
  XZIStreamBuf buf_;
};

} // namespace linpipe
