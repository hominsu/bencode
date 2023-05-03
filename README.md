<!-- PROJECT SHIELDS -->
<p align="center">
<a href="#contributors-"><img src="https://img.shields.io/badge/all_contributors-1-orange.svg?style=for-the-badge" alt="Contributors"></a>
<a href="https://github.com/hominsu/bencode/network/members"><img src="https://img.shields.io/github/forks/hominsu/bencode.svg?style=for-the-badge" alt="Forks"></a>
<a href="https://github.com/hominsu/bencode/stargazers"><img src="https://img.shields.io/github/stars/hominsu/bencode.svg?style=for-the-badge" alt="Stargazers"></a>
<a href="https://github.com/hominsu/bencode/issues"><img src="https://img.shields.io/github/issues/hominsu/bencode.svg?style=for-the-badge" alt="Issues"></a>
<a href="https://github.com/hominsu/bencode/blob/master/LICENSE"><img src="https://img.shields.io/github/license/hominsu/bencode.svg?style=for-the-badge" alt="License"></a>
<a href="https://github.com/hominsu/bencode/actions/workflows/docker-publish.yml"><img src="https://img.shields.io/github/actions/workflow/status/hominsu/slink/go.yml?branch=main&style=for-the-badge" alt="Deploy"></a>
</p>


<!-- PROJECT LOGO -->
<br/>
<div align="center">

<h3 align="center">bencode</h3>

  <p align="center">
    A header-only Bencode parser and generator for C++17. It supports both SAX and DOM style API.
    <br/>
    <a href="https://me.hauhau.cn/projects/bencode/"><strong>Explore the docs »</strong></a>
    <br/>
    <br/>
    <a href="#Features">Features</a>
    ·
    <a href="#Examples">Examples</a>
    ·
    <a href="#Building">Building</a>
    ·
    <a href="#Integration">Integration</a>
    .
    <a href="#License">License</a>
  </p>

</div>

##### Translations: [English](README.md) | [简体中文](README_zh.md)

## Features

- **Simple, Fast**. bencode library contains only header files, does not rely on Boost. You can "assemble" bencode library's Handler at will.
- **API simplicity**. bencode library supports both DOM and SAX-style API, where SAX allows custom handlers for streaming processing.
- **Multiple input/output streams**. bencode library has built-in string input/output streams and file input/output streams, and make full use of memory buffers to improve read and write speed.
- **STD Streams Wrapper**. bencode library provides official wrappers for std::istream and std::ostream, can be combined with bencode library's built-in input/output streams.

## Examples

### Usage at a glance

This simple example parses a Bencode string into a document (DOM), make a simple modification of the DOM, and finally stringify the DOM to a Bencode string.

```cpp
#include <cstdio>
#include <cstdlib>

#include "bencode/document.h"
#include "bencode/exception.h"
#include "bencode/writer.h"
#include "bencode/string_write_stream.h"
#include "sample.h"

int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  // 1. Parse a Bencode string into DOM.
  bencode::Document doc;
  auto err = doc.Parse(kSample[0]);
  if (err != bencode::error::OK) {
    puts(bencode::ParseErrorStr(err));
    return EXIT_FAILURE;
  }

  // 2. Modify it by DOM.
  auto &s = doc["creation date"];
  s.SetInteger(0);

  // 3. Stringify the DOM
  bencode::StringWriteStream os;
  bencode::Writer writer(os);
  doc.WriteTo(writer);

  // Output
  fprintf(stdout, "%.*s", static_cast<int>(os.get().length()), os.get().data());

  return 0;
}
```

Output:

```text
d8:announce41:http://bttracker.debian.org:6969/announce7:comment35:"Debian CD from cdimage.debian.org"10:created by13:mktorrent 1.113:creation datei0e4:infod6:lengthi3909091328e4:name29:debian-11.6.0-amd64-DVD-1.iso12:piece lengthi262144e6:pieces41:(binary blob of the hashes of each piece)ee
```

### Conversion between Bencode and JSON

Due to the excellent design of bencode library, the bencode library can be easily combined with [neujson](https://github.com/hominsu/neujson) to achieve the mutual conversion of JSON and Bencode. Also, this is an example of SAX. Unlike DOM, SAX does not store data into memory, but writes directly from the input stream to the output stream

#### Bencode to JSON

```c++
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
 private:
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
```

Output:

```json
{
  "announce": "http://bttracker.debian.org:6969/announce",
  "comment": "\"Debian CD from cdimage.debian.org\"",
  "created by": "mktorrent 1.1",
  "creation date": 1671279452,
  "info": {
    "length": 3909091328,
    "name": "debian-11.6.0-amd64-DVD-1.iso",
    "piece length": 262144,
    "pieces": "(binary blob of the hashes of each piece)"
  }
}
```

#### JSON to Bencode

```c++
#include <memory>
#include <string_view>

#include "bencode/file_write_stream.h"
#include "bencode/writer.h"
#include "sample.h"

#include "neujson/internal/ieee754.h"
#include "neujson/non_copyable.h"
#include "neujson/reader.h"
#include "neujson/string_read_stream.h"

template<typename Handler>
class JSONToBencode : neujson::NonCopyable {
 private:
  Handler &handler_;

 public:
  explicit JSONToBencode(Handler &handler) : handler_(handler) {}

  bool Null() { return handler_.Null(); }
  bool Bool(bool b) {
    (void) b;
    return true;
  }
  bool Int32(int32_t i32) { return handler_.Integer(i32); }
  bool Int64(int64_t i64) { return handler_.Integer(i64); }
  bool Double(neujson::internal::Double d) {
    (void) d;
    return true;
  };
  bool String(std::string_view str) { return handler_.String(str); }
  bool Key(std::string_view str) { return handler_.Key(str); }
  bool StartArray() { return handler_.StartList(); }
  bool EndArray() { return handler_.EndList(); }
  bool StartObject() { return handler_.StartDict(); }
  bool EndObject() { return handler_.EndDict(); }
};

int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  neujson::StringReadStream in(kSample[1]);

  bencode::FileWriteStream out(stdout);
  bencode::Writer writer(out);
  JSONToBencode to_json(writer);

  auto err = neujson::Reader::Parse(in, to_json);
  if (err != neujson::error::OK) {
    puts(neujson::ParseErrorStr(err));
    return EXIT_FAILURE;
  }

  return 0;
}
```

Output:

```text
d8:announce41:http://bttracker.debian.org:6969/announce7:comment35:"Debian CD from cdimage.debian.org"10:created by13:mktorrent 1.113:creation datei1671279452e4:infod6:lengthi3909091328e4:name29:debian-11.6.0-amd64-DVD-1.iso12:piece lengthi262144e6:pieces41:(binary blob of the hashes of each piece)ee
```

## Building

This project requires C++17. This library uses following projects：

When building tests:

- [google/googletest](https://github.com/google/googletest)

When building examples:

- [neujson](https://github.com/hominsu/neujson)

All dependencies are automatically retrieved from github during building, and you do not need to configure them.

With the CMake build types, you can control whether examples and tests are built.

```bash
cmake -H. -Bbuild \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=/Users/hominsu/utils/install \
	-BENCODE_BUILD_EXAMPLES=ON \
	-BENCODE_BUILD_TESTS=ON
cmake --build ./build --parallel $(nproc)
ctest -VV --test-dir ./build/ --output-on-failure
cmake --install ./build
```

Or just installed as a CMake package.

```bash
cmake -H. -Bbuild \
	-BENCODE_BUILD_EXAMPLES=OFF \
	-BENCODE_BUILD_TESTS=OFF
cmake --install ./build
```

Uinstall

```bash
cd build
make uninstall
```

## Integration

Located with `find_package` in CMake.

```cmake
find_package(bencode REQUIRED)
target_include_directories(foo PUBLIC ${bencode_INCLUDE_DIRS})
```

## License

Distributed under the MIT license. See `LICENSE` for more information.
