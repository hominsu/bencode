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
// Created by Homin Su on 2023/3/12.
//

#include <memory>
#include <string_view>

#include "bencode/file_write_stream.h"
#include "bencode/writer.h"
#include "sample.h"

#include "neujson/internal/ieee754.h"
#include "neujson/non_copyable.h"
#include "neujson/reader.h"
#include "neujson/string_read_stream.h"

template <typename Handler> class JSONToBencode : neujson::NonCopyable {
  Handler &handler_;

public:
  explicit JSONToBencode(Handler &handler) : handler_(handler) {}

  bool Null() { return handler_.Null(); }
  bool Bool(const bool b) {
    (void)b;
    return true;
  }
  bool Int32(int32_t i32) { return handler_.Integer(i32); }
  bool Int64(int64_t i64) { return handler_.Integer(i64); }
  bool Double(const neujson::internal::Double d) {
    (void)d;
    return true;
  };
  bool String(std::string_view str) { return handler_.String(str); }
  bool Key(std::string_view str) { return handler_.Key(str); }
  bool StartArray() { return handler_.StartList(); }
  bool EndArray() { return handler_.EndList(); }
  bool StartObject() { return handler_.StartDict(); }
  bool EndObject() { return handler_.EndDict(); }
};

int main(const int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  neujson::StringReadStream in(kSample[1]);

  bencode::FileWriteStream out(stdout);
  bencode::Writer writer(out);
  JSONToBencode to_json(writer);

  if (const auto err = neujson::Reader::Parse(in, to_json);
      err != neujson::error::OK) {
    puts(neujson::ParseErrorStr(err));
    return EXIT_FAILURE;
  }

  return 0;
}
