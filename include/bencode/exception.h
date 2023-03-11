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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
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

#ifndef BENCODE_INCLUDE_BENCODE_EXCEPTION_H_
#define BENCODE_INCLUDE_BENCODE_EXCEPTION_H_

#include <exception>

#include "bencode.h"

namespace bencode {

#define ERR_TABLE(ERR)  \
  ERR(OK, "ok")         \
  ERR(ROOT_NOT_SINGULAR, "root not singular") \
  ERR(BAD_VALUE, "bad value")                 \
  ERR(EXPECT_VALUE, "expect value")           \
  ERR(NUMBER_TOO_BIG, "number too big")       \
  ERR(MISS_STRING_LENGTH, "miss string length") \
  ERR(MISS_INITIAL_I, "miss initial i")   \
  ERR(MISS_INITIAL_L, "miss initial l")   \
  ERR(MISS_INITIAL_D, "miss initial d")   \
  ERR(MISS_TRAILING_E, "miss trailing e") \
  ERR(MISS_KEY, "miss key")               \
  ERR(MISS_COLON, "miss colon")           \
  ERR(USER_STOPPED, "user stopped Parse") \
  //

namespace error {
enum ParseError {
#define ERR_NO(_err, _str) _err,
  ERR_TABLE(ERR_NO)
#undef ERR_NO
};
} // namespace error

inline const char *ParseErrorStr(error::ParseError err) {
  const static char *err_str_table[] = {
#define ERR_STR(_err, _str) _str,
      ERR_TABLE(ERR_STR)
#undef ERR_STR
  };

  BENCODE_ASSERT(err >= 0 && err < sizeof(err_str_table) / sizeof(err_str_table[0]));
  return err_str_table[err];
}

#undef ERR_TABLE

class Exception : public std::exception {
 private:
  error::ParseError err_;

 public:
  explicit Exception(error::ParseError err) : err_(err) {}

  [[nodiscard]] const char *what() const noexcept override { return ParseErrorStr(err_); }
  [[nodiscard]] error::ParseError err() const { return err_; }
};

} // namespace bencode

#endif //BENCODE_INCLUDE_BENCODE_EXCEPTION_H_
