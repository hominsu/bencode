// MIT License
//
// Copyright (c) 2023 HominSu
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// Created by Homin Su on 2023/3/10.
//

#ifndef BENCODE_INCLUDE_BENCODE_STRING_WRITE_STREAM_H_
#define BENCODE_INCLUDE_BENCODE_STRING_WRITE_STREAM_H_

#include <string_view>
#include <vector>

#include "bencode.h"
#include "non_copyable.h"

#if defined(__GNUC__)
BENCODE_DIAG_PUSH
BENCODE_DIAG_OFF(effc++)
#endif

namespace bencode {

class StringWriteStream : NonCopyable {
public:
  using Ch = char;

private:
  static const std::size_t kInnerBufferSize = 256;
  std::vector<Ch> buffer_;

public:
  StringWriteStream() { buffer_.reserve(kInnerBufferSize); }

  void put(Ch ch) { buffer_.push_back(ch); }

  void puts(const Ch *str, std::size_t length) {
    buffer_.insert(buffer_.end(), str, str + length);
  }

  void put_sv(std::string_view str) {
    buffer_.insert(buffer_.end(), str.begin(), str.end());
  }

  [[nodiscard]] std::string_view get() const {
    return {buffer_.data(), buffer_.size()};
  }

  void flush() {}
};

} // namespace bencode

#if defined(__GNUC__)
BENCODE_DIAG_POP
#endif

#endif // BENCODE_INCLUDE_BENCODE_STRING_WRITE_STREAM_H_
