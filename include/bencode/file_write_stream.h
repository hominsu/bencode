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

#ifndef BENCODE_INCLUDE_BENCODE_FILE_WRITE_STREAM_H_
#define BENCODE_INCLUDE_BENCODE_FILE_WRITE_STREAM_H_

#include <cstdio>
#include <cstring>

#include <string_view>

#include "bencode.h"
#include "non_copyable.h"

namespace bencode {

class FileWriteStream : NonCopyable {
  static constexpr std::size_t kInnerBufferSize = 256;
  std::FILE *fp_;
  char inner_buffer_[kInnerBufferSize]{};
  char *buffer_;
  char *buffer_end_;
  char *current_;

public:
  explicit FileWriteStream(std::FILE *fp)
      : fp_(fp), buffer_(inner_buffer_),
        buffer_end_(buffer_ + kInnerBufferSize), current_(buffer_) {
    BENCODE_ASSERT(fp != nullptr && "FILE pointer equal zero");
  }

  explicit FileWriteStream(std::FILE *fp, char *buffer, const std::size_t buffer_size)
      : fp_(fp), buffer_(buffer), buffer_end_(buffer + buffer_size),
        current_(buffer_) {
    BENCODE_ASSERT(fp != nullptr && "FILE pointer equal zero");
  }

  template <std::size_t N>
  explicit FileWriteStream(std::FILE *fp, char (&buffer)[N])
      : fp_(fp), buffer_(buffer), buffer_end_(buffer + N), current_(buffer_) {
    BENCODE_ASSERT(fp != nullptr && "FILE pointer equal zero");
  }

  ~FileWriteStream() {
    if (current_ != buffer_) {
      flush();
    }
  }

  void put(const char ch) {
    if (current_ >= buffer_end_) {
      flush();
    }
    *current_++ = ch;
  }

  void put_n(const char ch, std::size_t size) {
    auto avail = static_cast<std::size_t>(buffer_end_ - current_);
    while (size > avail) {
      std::memset(current_, ch, avail);
      current_ += avail;
      flush();
      size -= avail;
      avail = static_cast<std::size_t>(buffer_end_ - current_);
    }
    if (size > 0) {
      std::memset(current_, ch, size);
      current_ += size;
    }
  }

  void puts(const char *str, std::size_t length) {
    auto avail = static_cast<std::size_t>(buffer_end_ - current_);
    std::size_t copy = 0;

    while (length > avail) {
      std::memcpy(current_, str + copy, avail);
      current_ += avail;
      copy += avail;
      flush();
      length -= avail;
      avail = static_cast<std::size_t>(buffer_end_ - current_);
    }

    if (length > 0) {
      std::memcpy(current_, str + copy, length);
      current_ += length;
    }
  }

  void put_sv(const std::string_view sv) {
    auto avail = static_cast<std::size_t>(buffer_end_ - current_);
    std::size_t length = sv.length();
    std::size_t copy = 0;

    while (length > avail) {
      std::memcpy(current_, sv.data() + copy, avail);
      current_ += avail;
      copy += avail;
      flush();
      length -= avail;
      avail = static_cast<std::size_t>(buffer_end_ - current_);
    }

    if (length > 0) {
      std::memcpy(current_, sv.data() + copy, length);
      current_ += length;
    }
  }

  void flush() {
    if (current_ != buffer_) {
      size_t result = std::fwrite(
          buffer_, 1, static_cast<std::size_t>(current_ - buffer_), fp_);
      if (result < static_cast<std::size_t>(current_ - buffer_)) {
      }
      current_ = buffer_;
    }
  }
};

} // namespace bencode

#endif // BENCODE_INCLUDE_BENCODE_FILE_WRITE_STREAM_H_
