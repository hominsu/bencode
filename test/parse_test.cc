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

#include <cstdint>

#include "bencode/bencode.h"
#include "bencode/document.h"
#include "bencode/exception.h"
#include "bencode/non_copyable.h"
#include "bencode/reader.h"
#include "bencode/string_read_stream.h"
#include "bencode/value.h"

#include "gtest/gtest.h"

#if defined(_MSC_VER) && !defined(__clang__)
BENCODE_DIAG_PUSH
#elif defined(__GNUC__)
BENCODE_DIAG_PUSH
BENCODE_DIAG_OFF(effc++)
#endif

class TestHandler : bencode::NonCopyable {
  bencode::Value value_;
  bencode::Type value_type_ = bencode::B_NULL;

  bool test_error_ = false;

public:
  bool Null();
  bool Integer(int64_t i64);
  bool String(std::string_view str);
  bool Key(std::string_view str);
  bool StartList();
  bool EndList();
  bool StartDict();
  bool EndDict();

  [[nodiscard]] bencode::Type type() const { return value_type_; }
  [[nodiscard]] bencode::Value value() const { return value_; }

  void set_test_error(const bool test_error) { test_error_ = test_error; }

private:
  void AddValue(bencode::Value &&value) {
    if (!test_error_) {
      value_type_ = value.GetType();
    }
    value_ = std::move(value);
  }
};

bool TestHandler::Null() {
  AddValue(bencode::Value(bencode::B_NULL));
  return true;
}

bool TestHandler::Integer(const int64_t i64) {
  AddValue(bencode::Value(i64));
  return true;
}

bool TestHandler::String(const std::string_view str) {
  AddValue(bencode::Value(str));
  return true;
}

bool TestHandler::Key(const std::string_view str) {
  AddValue(bencode::Value(str));
  return true;
}

bool TestHandler::StartList() {
  AddValue(bencode::Value(bencode::B_LIST));
  return true;
}

bool TestHandler::EndList() {
  AddValue(bencode::Value(bencode::B_LIST));
  return true;
}

bool TestHandler::StartDict() {
  AddValue(bencode::Value(bencode::B_DICT));
  return true;
}

bool TestHandler::EndDict() {
  AddValue(bencode::Value(bencode::B_DICT));
  return true;
}

#define ERROR_EQ(expect, actual)                                               \
  do {                                                                         \
    EXPECT_STREQ(bencode::ParseErrorStr((expect)),                             \
                 bencode::ParseErrorStr((actual)));                            \
  } while (0) //

#define TEST_INTEGER(expect, b)                                                \
  do {                                                                         \
    std::string_view ss((b));                                                  \
    bencode::StringReadStream read_stream(ss);                                 \
    TestHandler test_handler;                                                  \
    ERROR_EQ(bencode::error::ParseError::OK,                                   \
             bencode::Reader::Parse(read_stream, test_handler));               \
    EXPECT_EQ(bencode::B_INTEGER, test_handler.type());                        \
    EXPECT_EQ((expect), test_handler.value().GetInteger());                    \
  } while (0) //

TEST(parse, int32) {
  TEST_INTEGER(0, "i0e");
  TEST_INTEGER(-0, "i-0e");
  TEST_INTEGER(1234567890, "i1234567890e");
  TEST_INTEGER(-1234567890, "i-1234567890e");
  TEST_INTEGER(INT32_MAX, "i2147483647e");
  TEST_INTEGER(INT32_MIN, "i-2147483648e");
}

TEST(parse, int64) {
  TEST_INTEGER(0, "i0e");
  TEST_INTEGER(-0, "i-0e");
  TEST_INTEGER(12345678901234, "i12345678901234e");
  TEST_INTEGER(-12345678901234, "i-12345678901234e");
  TEST_INTEGER(INT64_MAX, "i9223372036854775807e");
  TEST_INTEGER(INT64_MIN, "i-9223372036854775808e");
}

#define TEST_STRING(expect, b)                                                 \
  do {                                                                         \
    std::string_view ss((b));                                                  \
    bencode::StringReadStream read_stream(ss);                                 \
    TestHandler test_handler;                                                  \
    ERROR_EQ(bencode::error::ParseError::OK,                                   \
             bencode::Reader::Parse(read_stream, test_handler));               \
    EXPECT_EQ(bencode::B_STRING, test_handler.type());                         \
    EXPECT_STREQ((expect), test_handler.value().GetString().c_str());          \
  } while (0) //

TEST(parse, string) {
  TEST_STRING("", R"(0:)");
  TEST_STRING("Hello", R"(5:Hello)");
}

TEST(parse, list) {
  {
    std::string_view ss("le");
    bencode::StringReadStream read_stream(ss);
    TestHandler test_handler;
    EXPECT_STREQ(bencode::ParseErrorStr(bencode::error::ParseError::OK),
                 bencode::ParseErrorStr(
                     bencode::Reader::Parse(read_stream, test_handler)));
    EXPECT_EQ(bencode::B_LIST, test_handler.type());
    EXPECT_EQ(0UL, test_handler.value().GetList()->size());
  }
  {
    std::string_view ss(R"(li123e3:abce)");
    bencode::Document doc;
    ERROR_EQ(bencode::error::ParseError::OK, doc.Parse(ss));
    EXPECT_EQ(bencode::B_LIST, doc.GetType());
    EXPECT_EQ(2UL, doc.GetList()->size());
    EXPECT_EQ(bencode::Type::B_INTEGER, doc.GetList()->at(0).GetType());
    EXPECT_EQ(bencode::Type::B_STRING, doc.GetList()->at(1).GetType());
    EXPECT_EQ(123, doc.GetList()->at(0).GetInteger());
    EXPECT_STREQ("abc", doc.GetList()->at(1).GetString().c_str());
  }
  {
    std::string_view ss(R"(lleli0eeli0ei1eeli0ei1ei2eee)");
    bencode::Document doc;
    ERROR_EQ(bencode::error::ParseError::OK, doc.Parse(ss));
    EXPECT_EQ(bencode::B_LIST, doc.GetType());
    EXPECT_EQ(4UL, doc.GetList()->size());
    for (std::size_t i = 0; i < 4; i++) {
      auto a = doc.GetList()->at(i);
      EXPECT_EQ(bencode::Type::B_LIST, a.GetType());
      EXPECT_EQ(i, a.GetList()->size());
      for (std::size_t j = 0; j < i; j++) {
        auto e = a.GetList()->at(j);
        EXPECT_EQ(bencode::Type::B_INTEGER, e.GetType());
        EXPECT_EQ(static_cast<int64_t>(j), e.GetInteger());
      }
    }
  }
}

TEST(parse, dict) {
  {
    std::string_view ss("de");
    bencode::StringReadStream read_stream(ss);
    TestHandler test_handler;
    ERROR_EQ(bencode::error::ParseError::OK,
             bencode::Reader::Parse(read_stream, test_handler));
    EXPECT_EQ(bencode::B_DICT, test_handler.type());
    EXPECT_EQ(0UL, test_handler.value().GetDict()->size());
  }
  {
    std::string_view ss(
        R"(d1:ii123e1:s3:abc1:lli1ei2ei3ee1:dd1:1i1e1:2i2e1:3i3eee)");
    bencode::Document doc;
    ERROR_EQ(bencode::error::ParseError::OK, doc.Parse(ss));
    EXPECT_EQ(bencode::B_DICT, doc.GetType());
    EXPECT_EQ(4UL, doc.GetDict()->size());

    EXPECT_STREQ("i", doc.GetDict()->at(0).key_.GetString().c_str());
    EXPECT_EQ(bencode::B_INTEGER, doc.GetDict()->at(0).value_.GetType());
    EXPECT_EQ(123, doc.GetDict()->at(0).value_.GetInteger());

    EXPECT_STREQ("s", doc.GetDict()->at(1).key_.GetString().c_str());
    EXPECT_EQ(bencode::B_STRING, doc.GetDict()->at(1).value_.GetType());
    EXPECT_STREQ("abc", doc.GetDict()->at(1).value_.GetString().c_str());

    EXPECT_STREQ("l", doc.GetDict()->at(2).key_.GetString().c_str());
    EXPECT_EQ(bencode::B_LIST, doc.GetDict()->at(2).value_.GetType());
    EXPECT_EQ(3UL, doc.GetDict()->at(2).value_.GetList()->size());
    for (std::size_t i = 0; i < 3; i++) {
      auto e = doc.GetDict()->at(2).value_.GetList();
      EXPECT_EQ(bencode::B_INTEGER, e->at(i).GetType());
      EXPECT_EQ(static_cast<int64_t>(i + 1), e->at(i).GetInteger());
    }

    EXPECT_STREQ("d", doc.GetDict()->at(3).key_.GetString().c_str());
    EXPECT_EQ(bencode::B_DICT, doc.GetDict()->at(3).value_.GetType());
    EXPECT_EQ(3UL, doc.GetDict()->at(3).value_.GetDict()->size());
    for (std::size_t i = 0; i < 3; i++) {
      auto object_key = doc.GetDict()->at(3).value_.GetDict()->at(i).key_;
      auto object_value = doc.GetDict()->at(3).value_.GetDict()->at(i).value_;
      char s[2]{};
      s[0] = static_cast<char>('1' + i);
      EXPECT_STREQ(s, object_key.GetString().c_str());
      EXPECT_EQ(1UL, object_key.GetString().size());
      EXPECT_EQ(bencode::B_INTEGER, object_value.GetType());
      EXPECT_EQ(static_cast<int64_t>(i + 1), object_value.GetInteger());
    }
  }
}

#define TEST_PARSE_ERROR(error, b)                                             \
  do {                                                                         \
    bencode::StringReadStream read_stream(b);                                  \
    TestHandler test_handler;                                                  \
    ERROR_EQ((error), bencode::Reader::Parse(read_stream, test_handler));      \
  } while (0) //

TEST(parse, bad_value) {
  TEST_PARSE_ERROR(bencode::error::BAD_VALUE, "i-e");  // minus only
  TEST_PARSE_ERROR(bencode::error::BAD_VALUE, "i-ae"); // minus and invalid
}

TEST(parse, expect_value) {
  TEST_PARSE_ERROR(bencode::error::EXPECT_VALUE, "");
}

TEST(parse, number_too_big) {
  TEST_PARSE_ERROR(bencode::error::NUMBER_TOO_BIG,
                   "i92233791812123120312116854775808e"); // positive overflow
  TEST_PARSE_ERROR(bencode::error::NUMBER_TOO_BIG,
                   "i-92233791812123120312116854775808e"); // negative overflow
  TEST_PARSE_ERROR(
      bencode::error::NUMBER_TOO_BIG,
      "i-9223372036854775809e"); // overflow after conversion to signed
}

TEST(parse, miss_trailing_e) {
  TEST_PARSE_ERROR(bencode::error::MISS_TRAILING_E, "i1.23e"); // invalid number
  TEST_PARSE_ERROR(bencode::error::MISS_TRAILING_E, "i000123e"); // leading zero
}

TEST(parse, miss_key) {
  TEST_PARSE_ERROR(bencode::error::MISS_KEY, "ddee");
  TEST_PARSE_ERROR(bencode::error::MISS_KEY, "dlee");
  TEST_PARSE_ERROR(bencode::error::MISS_KEY, "di1ee");
}

TEST(parse, miss_colon) {
  TEST_PARSE_ERROR(bencode::error::MISS_COLON, "1");
  TEST_PARSE_ERROR(bencode::error::MISS_COLON, "1a");
}

#if defined(__GNUC__) || (defined(_MSC_VER) && !defined(__clang__))
BENCODE_DIAG_POP
#endif