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

#include <cstdlib>

#include <sstream>

#include "bencode/exception.h"
#include "bencode/document.h"
#include "bencode/istream_wrapper.h"
#include "bencode/ostream_wrapper.h"
#include "bencode/writer.h"
#include "sample.h"

int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  std::stringstream iss;
  std::stringstream oss;

  iss << kSample[0];

  // any class derived from std::istream, ex. std::istringstream, std::stringstream, std::ifstream, std::fstream
  bencode::IStreamWrapper is(iss);

  bencode::Document doc;
  auto err = doc.ParseStream(is);

  if (err != bencode::error::OK) {
    puts(bencode::ParseErrorStr(err));
    return EXIT_FAILURE;
  }

  // any class derived from std::ostream, ex. std::ostringstream, std::stringstream, std::ofstream, std::fstream
  bencode::OStreamWrapper os(oss);
  bencode::Writer writer(os);
  doc.WriteTo(writer);

  fprintf(stdout, "%.*s", static_cast<int>(oss.str().length()), oss.str().data());

  return 0;
}