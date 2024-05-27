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
// Created by Homin Su on 2023/3/9.
//

#ifndef BENCODE_INCLUDE_BENCODE_STRING_READ_STREAM_H_
#define BENCODE_INCLUDE_BENCODE_STRING_READ_STREAM_H_

#include <string_view>

#include "bencode.h"
#include "concepts.h"
#include "non_copyable.h"

namespace bencode {

class StringReadStream : NonCopyable {
public:
  using Iterator = std::string_view::iterator;

private:
  std::string_view bencode_;
  Iterator iter_;

public:
  explicit StringReadStream(std::string_view bencode)
      : bencode_(bencode), iter_(bencode_.begin()) {}

  [[nodiscard]] bool hasNext() const { return iter_ != bencode_.end(); }

  [[nodiscard]] char peek() const { return hasNext() ? *iter_ : '\0'; }

  char next() {
    if (hasNext()) {
      const char ch = *iter_;
      std::advance(iter_, 1);
      return ch;
    }
    return '\0';
  }

  template <concepts::PositiveNumber T> std::string_view next(T n) {
    auto start = iter_;
    if (std::distance(iter_, bencode_.end()) >= n) {
      std::advance(iter_, n);
    }
    return {start, iter_};
  }

  template <concepts::GreaterEqualZeroNumber T> void skip(T n) {
    if (std::distance(iter_, bencode_.end()) >= n) {
      std::advance(iter_, n);
    }
  }

  void assertNext(const char ch) {
    (void)ch;
    BENCODE_ASSERT(peek() == ch);
    next();
  }
};

} // namespace bencode

#endif // BENCODE_INCLUDE_BENCODE_STRING_READ_STREAM_H_
