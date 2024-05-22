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

#ifndef BENCODE_INCLUDE_BENCODE_ISTREAM_WRAPPER_H_
#define BENCODE_INCLUDE_BENCODE_ISTREAM_WRAPPER_H_

#include <sstream>

#include "bencode.h"
#include "non_copyable.h"

namespace bencode {

template <class Stream> class IStreamWrapper : NonCopyable {
public:
  using Ch = typename Stream::char_type;

private:
  static const std::size_t kInnerBufferSize = 256;
  Stream &stream_;
  Ch inner_buffer_[kInnerBufferSize]{};
  Ch *buffer_;
  Ch *current_;
  Ch *buffer_last_;
  std::size_t buffer_size_;
  std::size_t read_count_;
  std::size_t read_total_;
  bool eof_;

public:
  explicit IStreamWrapper(Stream &stream)
      : stream_(stream), buffer_(inner_buffer_), current_(inner_buffer_),
        buffer_last_(nullptr), buffer_size_(kInnerBufferSize), read_count_(0),
        read_total_(0), eof_(false) {
    read();
  }

  IStreamWrapper(Stream &stream, char *buffer, std::size_t buffer_size)
      : stream_(stream), buffer_(buffer), current_(buffer_),
        buffer_last_(nullptr), buffer_size_(buffer_size), read_count_(0),
        read_total_(0), eof_(false) {
    BENCODE_ASSERT(buffer_size_ >= 4 &&
                   "buffer size should be bigger then four");
    read();
  }

  [[nodiscard]] bool hasNext() const {
    return !eof_ || (current_ + 1 - !eof_ <= buffer_last_);
  }

  Ch peek() { return *current_; }

  Ch next() {
    Ch ch = *current_;
    read();
    return ch;
  }

  template <typename Tint, class = typename std::enable_if_t<std::is_integral_v<
                               std::remove_reference_t<Tint>>>>
  void next(Tint n) {
    for (Tint i = 0; i < n; ++i) {
      if (hasNext()) {
        read();
      } else {
        break;
      }
    }
  }

  void assertNext(char ch) {
    (void)ch;
    BENCODE_ASSERT(peek() == ch);
    read();
  }

private:
  void read() {
    if (current_ < buffer_last_) {
      ++current_;
    } else if (!eof_) {
      read_total_ += read_count_;

      // if no eof
      buffer_last_ = buffer_ + buffer_size_ - 1;
      current_ = buffer_;

      // eof
      if (!stream_.read(buffer_, static_cast<std::streamsize>(buffer_size_))) {
        read_count_ = static_cast<std::size_t>(stream_.gcount());
        *(buffer_last_ = buffer_ + read_count_) = '\0';
        eof_ = true;
      }
    }
  }
};

} // namespace bencode

#endif // BENCODE_INCLUDE_BENCODE_ISTREAM_WRAPPER_H_
