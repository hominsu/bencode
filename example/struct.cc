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

#include <cassert>
#include <cstdio>

#include <memory>
#include <string>

#include "bencode/document.h"
#include "bencode/file_write_stream.h"
#include "bencode/value.h"
#include "bencode/writer.h"
#include "sample.h"

struct TorrentFile {
  struct Info {
    int64_t length_{};
    std::string name_;
    int64_t piece_length_{};
    std::string pieces_;
  };

  std::string announce_;
  std::string comment_;
  int64_t creation_date_{};
  Info info_;

  explicit TorrentFile(const bencode::Value &val) {
    assert(val.GetType() == bencode::Type::B_DICT && "bad type");
    announce_ = val["announce"].GetString();
    comment_ = val["comment"].GetString();
    creation_date_ = val["creation date"].GetInteger();

    auto info = val["info"];
    info_.length_ = info["length"].GetInteger();
    info_.name_ = info["name"].GetString();
    info_.piece_length_ = info["piece length"].GetInteger();
    info_.pieces_ = info["pieces"].GetString();
  }

  [[nodiscard]] bencode::Value toBencode() const {
    auto value = bencode::Value(bencode::Type::B_DICT);
    value.AddMember("announce", announce_);
    value.AddMember("comment", comment_);
    value.AddMember("creation date", creation_date_);
    auto info = bencode::Value(bencode::Type::B_DICT);
    info.AddMember("length", info_.length_);
    info.AddMember("name", info_.name_);
    info.AddMember("piece length", info_.piece_length_);
    info.AddMember("pieces", info_.pieces_);
    value.AddMember("info", std::move(info));
    return value;
  }
};

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  // 1. Parse a Bencode string into DOM.
  bencode::Document doc;
  if (const auto err = doc.Parse(kSample[0]); err != bencode::error::OK) {
    puts(bencode::ParseErrorStr(err));
    return EXIT_FAILURE;
  }

  // 2. Bind DOM to struct.
  auto torrent_file = TorrentFile(doc);

  // 3. Convert struct to Bencode and output
  auto value = torrent_file.toBencode();
  char writeBuffer[65536];
  bencode::FileWriteStream out(stdout, writeBuffer);
  bencode::Writer writer(out);
  value.WriteTo(writer);

  return 0;
}