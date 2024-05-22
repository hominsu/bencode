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
// Created by Homin Su on 2023/3/11.
//

#include <memory>
#include <string_view>

#include "bencode/non_copyable.h"
#include "bencode/reader.h"
#include "bencode/string_read_stream.h"
#include "sample.h"

#include "neujson/file_write_stream.h"
#include "neujson/pretty_writer.h"

template<typename Handler>
class BencodeToJSON : bencode::NonCopyable {
  Handler &handler_;

 public:
  explicit BencodeToJSON(Handler &handler) : handler_(handler) {}

  bool Null() { return true; }
  bool Integer(int64_t i64) { return handler_.Int64(i64); }
  bool String(std::string_view str) { return handler_.String(str); }
  bool Key(std::string_view str) { return handler_.Key(str); }
  bool StartList() { return handler_.StartArray(); }
  bool EndList() { return handler_.EndArray(); }
  bool StartDict() { return handler_.StartObject(); }
  bool EndDict() { return handler_.EndObject(); }
};

int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  bencode::StringReadStream in(kSample[0]);

  neujson::FileWriteStream out(stdout);
  neujson::PrettyWriter pretty_writer(out);
  pretty_writer.SetIndent(' ', 2);
  BencodeToJSON to_json(pretty_writer);

  auto err = bencode::Reader::Parse(in, to_json);
  if (err != bencode::error::OK) {
    puts(bencode::ParseErrorStr(err));
    return EXIT_FAILURE;
  }

  return 0;
}
