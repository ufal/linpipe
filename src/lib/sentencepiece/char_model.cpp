// Copyright 2016 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.!

#include "lib/sentencepiece/char_model.h"
#include "lib/sentencepiece/util.h"

namespace linpipe::sentencepiece {
namespace character {

Model::Model(const ModelProto &model_proto) {
  model_proto_ = &model_proto;
  InitializePieces();
}

Model::~Model() {}

EncodeResult Model::Encode(absl::string_view normalized) const {
  if (!status().ok() || normalized.empty()) {
    return {};
  }

  // Splits the input into character sequence
  EncodeResult output;
  while (!normalized.empty()) {
    const int mblen = matcher_->PrefixMatch(normalized);
    linpipe::sentencepiece::absl::string_view w(normalized.data(), mblen);
    output.emplace_back(w, PieceToId(w));
    normalized.remove_prefix(mblen);
  }

  return output;
}

}  // namespace character
}  // namespace sentencepiece
