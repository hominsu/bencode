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
// Created by Homin Su on 2023/3/10.
//

#include <cstdio>
#include <cstdlib>

#include "bencode/document.h"
#include "bencode/file_read_stream.h"
#include "bencode/file_write_stream.h"
#include "bencode/writer.h"

int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

#if defined(_MSC_VER)
  std::FILE *input;
  fopen_s(&input, "../../debian-11.6.0-amd64-DVD-1.iso.torrent", "r");
#else
  std::FILE *input = fopen("../../debian-11.6.0-amd64-DVD-1.iso.torrent", "r");
#endif
  if (input == nullptr) { exit(EXIT_FAILURE); }
  bencode::FileReadStream is(input);

  bencode::Document doc;
  auto err = doc.ParseStream(is);
  fclose(input);

  if (err != bencode::error::OK) {
    puts(bencode::ParseErrorStr(err));
    return EXIT_FAILURE;
  }

  char writeBuffer[65536];
  bencode::FileWriteStream os(stdout, writeBuffer, sizeof(writeBuffer));
  bencode::Writer writer(os);
  doc.WriteTo(writer);

  return 0;
}
