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

#ifndef BENCODE_INCLUDE_BENCODE_DOCUMENT_H_
#define BENCODE_INCLUDE_BENCODE_DOCUMENT_H_

#include <cstdint>

#include <string_view>
#include <variant>
#include <vector>

#include "bencode.h"
#include "exception.h"
#include "reader.h"
#include "string_read_stream.h"
#include "value.h"

#ifdef __GNUC__
BENCODE_DIAG_PUSH
BENCODE_DIAG_OFF(effc++)
#endif // __GNUC__

namespace bencode {

class Document : public Value {
  struct Level {
    Value *value_;
    int value_count_;

    explicit Level(Value *value) : value_(value), value_count_(0) {}

    [[nodiscard]] Type type() const { return value_->type_; }
    [[nodiscard]] Value *last_value() const;
  };

  std::vector<Level> stack_;
  Value key_;
  bool see_value_ = false;

public:
  error::ParseError Parse(const char *bencode, std::size_t len);
  error::ParseError Parse(std::string_view bencode);

  template <required::read_stream::HasAllRequiredFunctions ReadStream>
  error::ParseError ParseStream(ReadStream &rs);

  // handler
  bool Null();
  bool Integer(int64_t i64);
  bool String(std::string_view str);
  bool Key(std::string_view str);
  bool StartList();
  bool EndList();
  bool StartDict();
  bool EndDict();

private:
  Value *AddValue(Value &&value);
};

inline Value *Document::Level::last_value() const {
  if (type() == B_LIST) {
    return &std::get<B_LIST_TYPE>(value_->data_)->back();
  } else {
    return &std::get<B_DICT_TYPE>(value_->data_)->back().value_;
  }
}

inline error::ParseError Document::Parse(const char *bencode,
                                         const std::size_t len) {
  return Parse(std::string_view(bencode, len));
}

inline error::ParseError Document::Parse(const std::string_view bencode) {
  StringReadStream rs(bencode);
  return ParseStream(rs);
}

template <required::read_stream::HasAllRequiredFunctions ReadStream>
error::ParseError Document::ParseStream(ReadStream &rs) {
  return Reader::Parse(rs, *this);
}

inline bool Document::Null() {
  AddValue(Value(B_NULL));
  return true;
}

inline bool Document::Integer(const int64_t i64) {
  AddValue(Value(i64));
  return true;
}

inline bool Document::String(const std::string_view str) {
  AddValue(Value(str));
  return true;
}

inline bool Document::Key(const std::string_view str) {
  AddValue(Value(str));
  return true;
}

inline bool Document::StartList() {
  stack_.emplace_back(AddValue(Value(B_LIST)));
  return true;
}

inline bool Document::EndList() {
  BENCODE_ASSERT(!stack_.empty());
  BENCODE_ASSERT(stack_.back().type() == B_LIST);
  stack_.pop_back();
  return true;
}

inline bool Document::StartDict() {
  stack_.emplace_back(AddValue(Value(B_DICT)));
  return true;
}

inline bool Document::EndDict() {
  BENCODE_ASSERT(!stack_.empty());
  BENCODE_ASSERT(stack_.back().type() == B_DICT);
  stack_.pop_back();
  return true;
}

inline Value *Document::AddValue(Value &&value) {
  auto type = value.GetType();
  (void)type;
  if (see_value_) {
    BENCODE_ASSERT(!stack_.empty() && "root not singular");
  } else {
    BENCODE_ASSERT(type_ == B_NULL);
    see_value_ = true;
    type_ = type;

    switch (type) {
    case B_NULL:
      break;
    case B_INTEGER:
      data_ = std::get<B_INTEGER_TYPE>(value.data_);
      break;
    case B_STRING:
      data_ = std::get<B_STRING_TYPE>(value.data_);
      std::get<B_STRING_TYPE>(value.data_) = nullptr;
      break;
    case B_LIST:
      data_ = std::get<B_LIST_TYPE>(value.data_);
      std::get<B_LIST_TYPE>(value.data_) = nullptr;
      break;
    case B_DICT:
      data_ = std::get<B_DICT_TYPE>(value.data_);
      std::get<B_DICT_TYPE>(value.data_) = nullptr;
      break;
    default:
      break;
    }
    value.type_ = B_NULL;
    return this;
  }

  if (auto &top = stack_.back(); top.type() == B_LIST) {
    top.value_->AddValue(value);
    ++top.value_count_;
    return top.last_value();
  } else {
    BENCODE_ASSERT(top.type() == B_DICT);
    if (top.value_count_ % 2 == 0) {
      BENCODE_ASSERT(type == B_STRING);
      key_ = std::move(value);
      ++top.value_count_;
      return &key_;
    } else {
      top.value_->AddMember(std::move(key_), std::move(value));
      ++top.value_count_;
      return top.last_value();
    }
  }
}

} // namespace bencode

#ifdef __GNUC__
BENCODE_DIAG_POP
#endif // __GNUC__

#endif // BENCODE_INCLUDE_BENCODE_DOCUMENT_H_
