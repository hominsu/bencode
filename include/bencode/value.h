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
// Created by Homin Su on 2023/3/8.
//

#ifndef BENCODE_INCLUDE_BENCODE_VALUE_H_
#define BENCODE_INCLUDE_BENCODE_VALUE_H_

#include <cstdint>
#include <cstring>

#include <algorithm>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "bencode.h"

namespace bencode {

namespace required::handler {
namespace details {

template <typename Handler>
concept HasNull = requires(Handler handler) {
  { handler.Null() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasInteger = requires(Handler handler, int64_t i) {
  { handler.Integer(i) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasString = requires(Handler handler, std::string_view sv) {
  { handler.String(sv) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasKey = requires(Handler handler, std::string_view sv) {
  { handler.Key(sv) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasStartList = requires(Handler handler) {
  { handler.StartList() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasEndList = requires(Handler handler) {
  { handler.EndList() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasStartDict = requires(Handler handler) {
  { handler.StartDict() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasEndDict = requires(Handler handler) {
  { handler.EndDict() } -> std::same_as<bool>;
};

} // namespace details

template <typename T>
concept HasAllRequiredFunctions =
    details::HasNull<T> && details::HasInteger<T> && details::HasString<T> &&
    details::HasKey<T> && details::HasStartList<T> && details::HasEndList<T> &&
    details::HasStartDict<T> && details::HasEndDict<T>;
} // namespace required::handler

#undef VALUE
#define VALUE(field, suffix)                                                   \
  field(NULL, std::monostate) suffix field(INTEGER, int64_t)                   \
  suffix field(STRING, std::shared_ptr<std::vector<char>>)                     \
      suffix field(LIST, std::shared_ptr<std::vector<Value>>)                  \
          suffix field(DICT, std::shared_ptr<std::vector<Member>>) //

class Value;
struct Member;

enum Type {
#undef VALUE_NAME
#define VALUE_NAME(_name, _type) B_##_name
#undef SUFFIX
#define SUFFIX ,
  VALUE(VALUE_NAME, SUFFIX)
#undef SUFFIX
#undef VALUE_NAME
};

using Data = std::variant<
#undef VALUE_TYPE
#define VALUE_TYPE(_name, _type) _type
#undef SUFFIX
#define SUFFIX ,
    VALUE(VALUE_TYPE, SUFFIX)
#undef SUFFIX
#undef VALUE_TYPE
    >;

class Document;

class Value {
public:
  using MemberIterator = std::vector<Member>::iterator;
  using ConstMemberIterator = std::vector<Member>::const_iterator;

#undef VALUE_TYPE
#define VALUE_TYPE(_name, _type) using B_##_name##_TYPE = _type;
#undef SUFFIX
#define SUFFIX
  VALUE(VALUE_TYPE, SUFFIX)
#undef SUFFIX
#undef VALUE_TYPE

private:
  friend class Document;

  using String = std::vector<char>;
  using List = std::vector<Value>;
  using Dict = std::vector<Member>;

  Type type_;
  Data data_;

public:
  explicit Value(Type type = B_NULL);
  explicit Value(B_INTEGER_TYPE i) : type_(B_INTEGER), data_(i) {};
  explicit Value(const char *s)
      : type_(B_STRING), data_(std::make_shared<String>(s, s + strlen(s))) {};
  explicit Value(const std::string_view sv)
      : type_(B_STRING),
        data_(std::make_shared<String>(sv.begin(), sv.end())) {};
  Value(const Value &value) = default;
  Value(Value &&value) noexcept
      : type_(value.type_), data_(std::move(value.data_)) {};
  ~Value() = default;

  [[nodiscard]] bool IsNull() const { return type_ == B_NULL; }
  [[nodiscard]] bool IsInteger() const { return type_ == B_INTEGER; }
  [[nodiscard]] bool IsString() const { return type_ == B_STRING; }
  [[nodiscard]] bool IsList() const { return type_ == B_LIST; }
  [[nodiscard]] bool IsDict() const { return type_ == B_DICT; }

  [[nodiscard]] std::size_t GetSize() const;
  [[nodiscard]] Type GetType() const;
  [[nodiscard]] B_INTEGER_TYPE GetInteger() const;
  [[nodiscard]] std::string_view GetStringView() const;
  [[nodiscard]] std::string GetString() const;
  [[nodiscard]] const auto &GetList() const;
  [[nodiscard]] const auto &GetDict() const;

  Value &SetInteger(B_INTEGER_TYPE i);
  Value &SetString(std::string_view sv);
  Value &SetList();
  Value &SetDict();

  MemberIterator MemberBegin();
  MemberIterator MemberEnd();
  MemberIterator FindMember(std::string_view key);

  [[nodiscard]] ConstMemberIterator MemberBegin() const;
  [[nodiscard]] ConstMemberIterator MemberEnd() const;
  [[nodiscard]] ConstMemberIterator FindMember(std::string_view key) const;

  Value &operator=(const Value &val);
  Value &operator=(Value &&val) noexcept;
  Value &operator[](std::size_t index);
  const Value &operator[](std::size_t index) const;
  Value &operator[](std::string_view key);
  const Value &operator[](std::string_view key) const;

  template <typename T> Value &AddValue(T &&value);

  template <typename T> Value &AddMember(const char *key, T &&value);
  Value &AddMember(Value &&key, Value &&value);

  template <required::handler::HasAllRequiredFunctions Handler>
  bool WriteTo(Handler &handler) const;
};

#undef VALUE

struct Member {
  Member(Value &&_key, Value &&_value)
      : key_(std::move(_key)), value_(std::move(_value)) {}
  Member(std::string_view _key, Value &&_value)
      : key_(_key), value_(std::move(_value)) {}

  Value key_;
  Value value_;
};

inline Value::Value(const Type type) : type_(type) {
  switch (type) {
  case B_NULL:
  case B_INTEGER:
    break;
  case B_STRING:
    data_ = std::make_shared<String>();
    break;
  case B_LIST:
    data_ = std::make_shared<List>();
    break;
  case B_DICT:
    data_ = std::make_shared<Dict>();
    break;
  default:
    BENCODE_ASSERT(false && "bad value GetType");
  }
}

inline std::size_t Value::GetSize() const {
  switch (type_) {
  case B_LIST:
    return std::get<B_LIST_TYPE>(data_)->size();
  case B_DICT:
    return std::get<B_DICT_TYPE>(data_)->size();
  default:
    return 1;
  }
}

inline Type Value::GetType() const { return type_; }

inline Value::B_INTEGER_TYPE Value::GetInteger() const {
  BENCODE_ASSERT(type_ == B_INTEGER);
  return std::get<B_INTEGER_TYPE>(data_);
}

inline std::string_view Value::GetStringView() const {
  BENCODE_ASSERT(type_ == B_STRING);
  auto s_ptr = std::get<B_STRING_TYPE>(data_).get();
  return {s_ptr->data(), s_ptr->size()};
}

inline std::string Value::GetString() const {
  BENCODE_ASSERT(type_ == B_STRING);
  return std::string(GetStringView());
}

inline const auto &Value::GetList() const {
  BENCODE_ASSERT(type_ == B_LIST);
  return std::get<B_LIST_TYPE>(data_);
}

inline const auto &Value::GetDict() const {
  BENCODE_ASSERT(type_ == B_DICT);
  return std::get<B_DICT_TYPE>(data_);
}

inline Value &Value::SetInteger(const B_INTEGER_TYPE i) {
  this->~Value();
  return *new (this) Value(i);
}

inline Value &Value::SetString(const std::string_view sv) {
  this->~Value();
  return *new (this) Value(sv);
}

inline Value &Value::SetList() {
  this->~Value();
  return *new (this) Value(B_LIST);
}

inline Value &Value::SetDict() {
  this->~Value();
  return *new (this) Value(B_DICT);
}

inline Value::MemberIterator Value::MemberBegin() {
  BENCODE_ASSERT(type_ == B_DICT);
  return std::get<B_DICT_TYPE>(data_)->begin();
}

inline Value::MemberIterator Value::MemberEnd() {
  BENCODE_ASSERT(type_ == B_DICT);
  return std::get<B_DICT_TYPE>(data_)->end();
}

inline Value::MemberIterator Value::FindMember(const std::string_view key) {
  BENCODE_ASSERT(type_ == B_DICT);
  return std::ranges::find_if(*std::get<B_DICT_TYPE>(data_),
                              [key](const Member &member) -> bool {
                                return member.key_.GetStringView() == key;
                              });
}

inline Value::ConstMemberIterator Value::MemberBegin() const {
  BENCODE_ASSERT(type_ == B_DICT);
  return const_cast<Value &>(*this).MemberBegin();
}

inline Value::ConstMemberIterator Value::MemberEnd() const {
  BENCODE_ASSERT(type_ == B_DICT);
  return const_cast<Value &>(*this).MemberEnd();
}

inline Value::ConstMemberIterator
Value::FindMember(std::string_view key) const {
  BENCODE_ASSERT(type_ == B_DICT);
  return const_cast<Value &>(*this).FindMember(key);
}

inline Value &Value::operator=(const Value &val) {
  BENCODE_ASSERT(this != &val);
  type_ = val.type_;
  data_ = val.data_;
  return *this;
}

inline Value &Value::operator=(Value &&val) noexcept {
  BENCODE_ASSERT(this != &val);
  type_ = val.type_;
  data_ = std::move(val.data_);
  val.type_ = B_NULL;
  return *this;
}

inline Value &Value::operator[](const std::size_t index) {
  BENCODE_ASSERT(type_ == B_LIST);
  return std::get<B_LIST_TYPE>(data_)->at(index);
}

inline const Value &Value::operator[](const std::size_t index) const {
  BENCODE_ASSERT(type_ == B_LIST);
  return std::get<B_LIST_TYPE>(data_)->at(index);
}

inline Value &Value::operator[](const std::string_view key) {
  BENCODE_ASSERT(type_ == B_DICT);
  if (const auto it = FindMember(key);
      it != std::get<B_DICT_TYPE>(data_)->end()) {
    return it->value_;
  }
  BENCODE_ASSERT(false && "value no found");
  static Value fake(B_NULL);
  return fake;
}

inline const Value &Value::operator[](const std::string_view key) const {
  BENCODE_ASSERT(type_ == B_DICT);
  return const_cast<Value &>(*this)[key];
}

template <typename T> Value &Value::AddValue(T &&value) {
  BENCODE_ASSERT(type_ == B_LIST);
  auto ptr = std::get<B_LIST_TYPE>(data_);
  ptr->emplace_back(std::forward<T>(value));
  return ptr->back();
}

template <typename T> Value &Value::AddMember(const char *key, T &&value) {
  return AddMember(Value(key), Value(std::forward<T>(value)));
}

inline Value &Value::AddMember(Value &&key, Value &&value) {
  BENCODE_ASSERT(type_ == B_DICT);
  BENCODE_ASSERT(key.type_ == B_STRING);
  BENCODE_ASSERT(FindMember(key.GetStringView()) == MemberEnd());
  auto ptr = std::get<B_DICT_TYPE>(data_);
  ptr->emplace_back(std::move(key), std::move(value));
  return ptr->back().value_;
}

#define CALL_HANDLER(expr)                                                     \
  do {                                                                         \
    if (!(expr)) {                                                             \
      return false;                                                            \
    }                                                                          \
  } while (false)

template <required::handler::HasAllRequiredFunctions Handler>
bool Value::WriteTo(Handler &handler) const {
  switch (type_) {
  case B_NULL:
    CALL_HANDLER(handler.Null());
    break;
  case B_INTEGER:
    CALL_HANDLER(handler.Integer(std::get<B_INTEGER_TYPE>(data_)));
    break;
  case B_STRING:
    CALL_HANDLER(handler.String(GetStringView()));
    break;
  case B_LIST:
    CALL_HANDLER(handler.StartList());
    for (auto &val : *GetList()) {
      CALL_HANDLER(val.WriteTo(handler));
    }
    CALL_HANDLER(handler.EndList());
    break;
  case B_DICT:
    CALL_HANDLER(handler.StartDict());
    for (auto &member : *GetDict()) {
      handler.Key(member.key_.GetStringView());
      CALL_HANDLER(member.value_.WriteTo(handler));
    }
    CALL_HANDLER(handler.EndDict());
    break;
  default:
    BENCODE_ASSERT(false && "bad type");
  }
  return true;
}

#undef CALL_HANDLER

} // namespace bencode

#endif // BENCODE_INCLUDE_BENCODE_VALUE_H_
