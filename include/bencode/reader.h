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

#ifndef BENCODE_INCLUDE_BENCODE_READER_H_
#define BENCODE_INCLUDE_BENCODE_READER_H_

#include <cstdint>

#include <string>

#include "exception.h"
#include "non_copyable.h"
#include "value.h"

namespace bencode {

class Reader : NonCopyable {
public:
  template <typename ReadStream, typename Handler>
  static error::ParseError Parse(ReadStream &rs, Handler &handler);

private:
  template <typename ReadStream, typename Handler>
  static void ParseInteger(ReadStream &rs, Handler &handler);

  template <typename ReadStream, typename Handler>
  static void ParseString(ReadStream &rs, Handler &handler, bool is_key);

  template <typename ReadStream, typename Handler>
  static void ParseList(ReadStream &rs, Handler &handler);

  template <typename ReadStream, typename Handler>
  static void ParseDict(ReadStream &rs, Handler &handler);

  template <typename ReadStream, typename Handler>
  static void ParseValue(ReadStream &rs, Handler &handler);

  static bool IsDigit(char ch) { return ch >= '0' && ch <= '9'; }
  static bool IsDigit1To9(char ch) { return ch >= '1' && ch <= '9'; }
};

template <typename ReadStream, typename Handler>
error::ParseError Reader::Parse(ReadStream &rs, Handler &handler) {
  try {
    ParseValue(rs, handler);
    if (rs.hasNext()) {
      throw Exception(error::ROOT_NOT_SINGULAR);
    }
    return error::OK;
  } catch (Exception &e) {
    return e.err();
  }
}

#define CALL(expr)                                                             \
  if (!(expr))                                                                 \
  throw Exception(error::USER_STOPPED)

template <typename ReadSteam, typename Handler>
void Reader::ParseInteger(ReadSteam &rs, Handler &handler) {
  if (rs.peek() == 'i') {
    rs.next();
  } else {
    throw Exception(error::MISS_INITIAL_I);
  }

  std::string buffer;

  if (rs.peek() == '+') {
    rs.next();
  }
  if (rs.peek() == '-') {
    buffer.push_back(rs.next());
  }
  if (rs.peek() == '0') {
    buffer.push_back(rs.next());
  } else {
    if (!IsDigit1To9(rs.peek())) {
      throw Exception(error::BAD_VALUE);
    }
    for (buffer.push_back(rs.next()); IsDigit(rs.peek());
         buffer.push_back(rs.next()))
      ;
  }

  if (buffer.empty()) {
    throw Exception(error::BAD_VALUE);
  }

  try {
    std::size_t idx;
    int64_t i64;
#if defined(__clang__) || defined(_MSC_VER)
    i64 = std::stoll(buffer, &idx, 10);
#elif defined(__GNUC__)
    i64 = __gnu_cxx::__stoa(&std::strtoll, "stoll", buffer.data(), &idx, 10);
#else
#error "complier no support"
#endif
    BENCODE_ASSERT(buffer.size() == idx);
    CALL(handler.Integer(i64));
  } catch (...) {
    throw Exception(error::NUMBER_TOO_BIG);
  }

  if (rs.peek() == 'e') {
    rs.next();
  } else {
    throw Exception(error::MISS_TRAILING_E);
  }
}

template <typename ReadSteam, typename Handler>
void Reader::ParseString(ReadSteam &rs, Handler &handler, bool is_key) {
  std::string buffer;

  if (rs.peek() == '0') {
    buffer.push_back(rs.next());
  } else {
    if (!IsDigit1To9(rs.peek())) {
      throw Exception(error::MISS_STRING_LENGTH);
    }
    for (buffer.push_back(rs.next()); IsDigit(rs.peek());
         buffer.push_back(rs.next()))
      ;
  }

  if (buffer.empty()) {
    throw Exception(error::MISS_STRING_LENGTH);
  }

  int64_t length;

  try {
    std::size_t idx;
#if defined(__clang__) || defined(_MSC_VER)
    length = std::stoll(buffer, &idx, 10);
#elif defined(__GNUC__)
    length = __gnu_cxx::__stoa(&std::strtoll, "stoll", buffer.data(), &idx, 10);
#else
#error "complier no support"
#endif
    BENCODE_ASSERT(buffer.size() == idx);
  } catch (...) {
    throw Exception(error::NUMBER_TOO_BIG);
  }

  if (rs.peek() == ':') {
    rs.next();
  } else {
    throw Exception(error::MISS_COLON);
  }

  std::string content;

  for (; length-- > 0; content.push_back(rs.next()))
    ;

  if (is_key) {
    CALL(handler.Key(std::move(content)));
  } else {
    CALL(handler.String(std::move(content)));
  }
}

template <typename ReadSteam, typename Handler>
void Reader::ParseList(ReadSteam &rs, Handler &handler) {
  CALL(handler.StartList());

  rs.assertNext('l');
  if (rs.peek() == 'e') {
    rs.next();
    CALL(handler.EndList());
    return;
  }

  while (true) {
    ParseValue(rs, handler);
    if (rs.peek() == 'e') {
      rs.next();
      CALL(handler.EndList());
      return;
    }
  }
}

template <typename ReadSteam, typename Handler>
void Reader::ParseDict(ReadSteam &rs, Handler &handler) {
  CALL(handler.StartDict());

  rs.assertNext('d');
  if (rs.peek() == 'e') {
    rs.next();
    CALL(handler.EndDict());
    return;
  }

  while (true) {
    // parse key
    if (!IsDigit(rs.peek())) {
      throw Exception(error::MISS_KEY);
    }
    ParseString(rs, handler, true);

    // parse element
    ParseValue(rs, handler);
    if (rs.peek() == 'e') {
      rs.next();
      CALL(handler.EndDict());
      return;
    }
  }
}

#undef CALL

template <typename ReadStream, typename Handler>
void Reader::ParseValue(ReadStream &rs, Handler &handler) {
  if (!rs.hasNext()) {
    throw Exception(error::EXPECT_VALUE);
  }

  switch (rs.peek()) {
  case 'd':
    return ParseDict(rs, handler);
  case 'i':
    return ParseInteger(rs, handler);
  case 'l':
    return ParseList(rs, handler);
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return ParseString(rs, handler, false);
  default:
    throw Exception(error::BAD_VALUE);
  }
}

} // namespace bencode

#endif // BENCODE_INCLUDE_BENCODE_READER_H_
