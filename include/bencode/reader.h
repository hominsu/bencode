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

namespace required::read_stream {
namespace details {

template <typename ReadStream>
concept HasHasNext = requires(ReadStream rs) {
  { rs.hasNext() } -> std::same_as<bool>;
};

template <typename ReadStream>
concept HasPeek = requires(ReadStream rs) {
  { rs.peek() } -> std::same_as<char>;
};

template <typename ReadStream>
concept HasNext = requires(ReadStream rs) {
  { rs.next() } -> std::same_as<char>;
};

template <typename ReadStream>
concept HasNextWithLength = requires(ReadStream rs, std::size_t n) {
  { rs.next(n) } -> std::same_as<std::string>;
} || requires(ReadStream rs, std::size_t n) {
  { rs.next(n) } -> std::same_as<std::string_view>;
};

template <typename ReadStream>
concept HasAssertNext = requires(ReadStream rs, char ch) {
  { rs.assertNext(ch) } -> std::same_as<void>;
};

} // namespace details

template <typename T>
concept HasAllRequiredFunctions =
    details::HasHasNext<T> && details::HasPeek<T> && details::HasNext<T> &&
    details::HasNextWithLength<T> && details::HasAssertNext<T>;

} // namespace required::read_stream

class Reader : NonCopyable {
public:
  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static error::ParseError Parse(ReadStream &rs, Handler &handler);

private:
  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseInteger(ReadStream &rs, Handler &handler);

  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseString(ReadStream &rs, Handler &handler, bool is_key);

  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseList(ReadStream &rs, Handler &handler);

  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseDict(ReadStream &rs, Handler &handler);

  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseValue(ReadStream &rs, Handler &handler);

  static bool IsDigit(const char ch) { return ch >= '0' && ch <= '9'; }
  static bool IsDigit1To9(const char ch) { return ch >= '1' && ch <= '9'; }
};

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
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

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
void Reader::ParseInteger(ReadStream &rs, Handler &handler) {
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

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
void Reader::ParseString(ReadStream &rs, Handler &handler, const bool is_key) {
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

  if (is_key) {
    CALL(handler.Key(rs.next(length)));
  } else {
    CALL(handler.String(rs.next(length)));
  }
}

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
void Reader::ParseList(ReadStream &rs, Handler &handler) {
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

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
void Reader::ParseDict(ReadStream &rs, Handler &handler) {
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

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
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
