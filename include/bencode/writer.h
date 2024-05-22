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

#ifndef BENCODE_INCLUDE_BENCODE_WRITER_H_
#define BENCODE_INCLUDE_BENCODE_WRITER_H_

#include <cstdint>

#include <string_view>
#include <vector>

#include "bencode.h"
#include "internal/itoa.h"
#include "non_copyable.h"
#include "value.h"

#if defined(__GNUC__)
BENCODE_DIAG_PUSH
BENCODE_DIAG_OFF(effc++)
#endif

namespace bencode {

template <typename WriteStream> class Writer : NonCopyable {
protected:
  uint64_t stack_;
  WriteStream &os_;

public:
  explicit Writer(WriteStream &os) : stack_(0), os_(os) {}
  virtual ~Writer() = default;

  virtual bool Null() { return true; }
  virtual bool Integer(int64_t i64) { return EndValue(WriteInteger(i64)); }
  virtual bool String(std::string_view str) {
    return EndValue(WriteString(str));
  }
  virtual bool Key(std::string_view str) { return EndValue(WriteKey(str)); }
  virtual bool StartList() {
    ++stack_;
    return EndValue(WriteStartList());
  }
  virtual bool EndList() {
    --stack_;
    return EndValue(WriteEndList());
  }
  virtual bool StartDict() {
    ++stack_;
    return EndValue(WriteStartDict());
  }
  virtual bool EndDict() {
    --stack_;
    return EndValue(WriteEndDict());
  }

protected:
  bool WriteInteger(int64_t i64);
  bool WriteString(std::string_view str);
  bool WriteKey(std::string_view str);
  bool WriteStartList();
  bool WriteEndList();
  bool WriteStartDict();
  bool WriteEndDict();
  bool EndValue(bool ret);
  void Flush() { os_.flush(); }
};

template <typename WriteStream>
bool Writer<WriteStream>::WriteInteger(int64_t i64) {
  char buf[32]{};
  auto size = static_cast<std::size_t>(internal::i64toa(i64, buf) - buf);
  os_.put('i');
  os_.puts(buf, size);
  os_.put('e');
  return true;
}

template <typename WriteStream>
bool Writer<WriteStream>::WriteString(std::string_view str) {
  char buf[32]{};
  auto size =
      static_cast<std::size_t>(internal::u64toa(str.length(), buf) - buf);
  os_.puts(buf, size);
  os_.put(':');
  os_.puts(str.data(), str.size());
  return true;
}

template <typename WriteStream>
bool Writer<WriteStream>::WriteKey(std::string_view str) {
  WriteString(str);
  return true;
}

template <typename WriteStream> bool Writer<WriteStream>::WriteStartList() {
  os_.put('l');
  return true;
}

template <typename WriteStream> bool Writer<WriteStream>::WriteEndList() {
  os_.put('e');
  return true;
}

template <typename WriteStream> bool Writer<WriteStream>::WriteStartDict() {
  os_.put('d');
  return true;
}

template <typename WriteStream> bool Writer<WriteStream>::WriteEndDict() {
  os_.put('e');
  return true;
}

template <typename WriteStream> bool Writer<WriteStream>::EndValue(bool ret) {
  // end of bencode text
  if (stack_ == 0) {
    Flush();
  }
  return ret;
}

} // namespace bencode

#if defined(__GNUC__)
BENCODE_DIAG_POP
#endif

#endif // BENCODE_INCLUDE_BENCODE_WRITER_H_
