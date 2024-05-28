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

#ifndef BENCODE_INCLUDE_BENCODE_FILE_READ_STREAM_H_
#define BENCODE_INCLUDE_BENCODE_FILE_READ_STREAM_H_

#include <cstdio>

#include "bencode.h"
#include "non_copyable.h"

namespace bencode {

class FileReadStream : NonCopyable {
  static constexpr std::size_t kInnerBufferSize = 256;
  std::FILE *fp_;
  char inner_buffer_[kInnerBufferSize]{};
  char *buffer_;
  char *current_;
  char *buffer_last_;
  std::size_t buffer_size_;
  std::size_t read_count_;
  std::size_t read_total_;
  bool eof_;

public:
  explicit FileReadStream(std::FILE *fp)
      : fp_(fp), buffer_(inner_buffer_), current_(inner_buffer_),
        buffer_last_(nullptr), buffer_size_(kInnerBufferSize), read_count_(0),
        read_total_(0), eof_(false) {
    BENCODE_ASSERT(fp_ != nullptr && "file pointer should not be empty");
    read();
  }

  explicit FileReadStream(std::FILE *fp, char *buffer,
                          const std::size_t buffer_size)
      : fp_(fp), buffer_(buffer), current_(buffer), buffer_last_(nullptr),
        buffer_size_(buffer_size), read_count_(0), read_total_(0), eof_(false) {
    BENCODE_ASSERT(fp_ != nullptr && "file pointer should not be empty");
    BENCODE_ASSERT(buffer_size_ >= 4 &&
                   "buffer size should be bigger then four");
    read();
  }

  template <std::size_t N>
  explicit FileReadStream(std::FILE *fp, char (&buffer)[N])
      : fp_(fp), buffer_(buffer), current_(buffer), buffer_last_(nullptr),
        buffer_size_(N), read_count_(0), read_total_(0), eof_(false) {
    BENCODE_ASSERT(fp_ != nullptr && "file pointer should not be empty");
    BENCODE_ASSERT(buffer_size_ >= 4 &&
                   "buffer size should be bigger then four");
    read();
  }

  [[nodiscard]] bool hasNext() const {
    return !eof_ || (current_ + 1 - !eof_ <= buffer_last_);
  }

  [[nodiscard]] char peek() const { return *current_; }

  char next() {
    const char ch = *current_;
    read();
    return ch;
  }

  std::string next(const std::size_t n) {
    auto str = std::make_shared<std::string>();
    auto ret = read(str, n);
    return {ret->c_str(), ret->size()};
  }

  void skip(const std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
      if (hasNext()) {
        read();
      } else {
        break;
      }
    }
  }

  void assertNext(const char ch) {
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
      read_count_ = std::fread(buffer_, 1, buffer_size_, fp_);
      buffer_last_ = buffer_ + read_count_ - 1;
      current_ = buffer_;

      if (read_count_ < buffer_size_) {
        buffer_[read_count_] = '\0';
        ++buffer_last_;
        eof_ = true;
      }
    }
  }

  std::shared_ptr<std::string> &read(std::shared_ptr<std::string> &str,
                                     const std::size_t n = 1) {
    if (current_ + n <= buffer_last_) {
      str->append(current_, n);
      current_ += n;
      return str;
    } else if (!eof_) {
      const std::size_t remaining = n - (buffer_last_ - current_ + 1);
      const std::size_t need_read = n - remaining;
      str->append(current_, buffer_last_ - current_ + 1);

      read_total_ += read_count_;
      read_count_ = std::fread(buffer_, 1, buffer_size_, fp_);
      buffer_last_ = buffer_ + read_count_ - 1;
      current_ = buffer_;

      if (read_count_ < buffer_size_) {
        buffer_[read_count_] = '\0';
        ++buffer_last_;
        eof_ = true;
      }

      if (eof_ && need_read > read_count_) {
        str->append(current_, read_count_);
        return str;
      }

      return read(str, need_read);
    }

    return str;
  }
};

} // namespace bencode

#endif // BENCODE_INCLUDE_BENCODE_FILE_READ_STREAM_H_
