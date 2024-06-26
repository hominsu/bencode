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

#ifndef BENCODE_INCLUDE_BENCODE_OSTREAM_WRAPPER_H_
#define BENCODE_INCLUDE_BENCODE_OSTREAM_WRAPPER_H_

#include <sstream>
#include <string_view>

#include "non_copyable.h"

namespace bencode {

template <class Stream> class OStreamWrapper : NonCopyable {
public:
  using Ch = typename Stream::char_type;

private:
  Stream &stream_;

public:
  explicit OStreamWrapper(Stream &stream) : stream_(stream) {}

  void put(Ch ch) { stream_.put(ch); }

  void puts(const char *str, std::streamsize length) {
    stream_.write(str, length);
  }

  void put_sv(std::string_view sv) const { stream_ << sv; }

  void flush() { stream_.flush(); }
};

} // namespace bencode

#endif // BENCODE_INCLUDE_BENCODE_OSTREAM_WRAPPER_H_
