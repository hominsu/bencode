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
    <a href="#特性">特性</a>
    ·
    <a href="#示例">示例</a>
    ·
    <a href="#构建">构建</a>
    ·
    <a href="#集成">集成</a>
    .
    <a href="#许可">许可</a>
  </p>

</div>

##### Translations: [English](README.md) | [简体中文](README_zh.md)

## 特性

- **简单、快速**. bencode 库只包含头文件, 不依赖于 Boost。你可以随意 "组装" bencode 库的 Handler。
- **简洁的 API**. bencode 库同时支持 DOM 和 SAX 风格的 API, SAX 可以自定义 Handler 实现流式处理。
- **多种输入输出流**. bencode 库内置了字符串输入输出流和文件输入输出流, 并充分利用内存缓冲区提高读写速度。
- **内置 std 流包装类**. bencode 库提供了正式的 std::istream 和 std::ostream 包装类, 可以与 bencode 库的内置输入输出流组合

## 示例

### 用法一目了然

此简单例子解析一个 Bencode 字符串至一个 document (DOM), 对 DOM 作出简单修改, 最终把 DOM 转换（stringify）至 Bencode 字符串。

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

输出:

```text
d8:announce41:http://bttracker.debian.org:6969/announce7:comment35:"Debian CD from cdimage.debian.org"10:created by13:mktorrent 1.113:creation datei0e4:infod6:lengthi3909091328e4:name29:debian-11.6.0-amd64-DVD-1.iso12:piece lengthi262144e6:pieces41:(binary blob of the hashes of each piece)ee
```

### Bencode 与 JSON 之间的相互转换

得益于 bencode 库优秀的设计。bencode 库可以很容易地与 [neujson](https://github.com/hominsu/neujson) 结合使用, 实现 JSON 与 Bencode 之间的相互转换。同时, 这也是 SAX 的一个示例。与 DOM 不同, SAX 不会将数据存储到内存中, 而是直接从输入流写到输出流中。

#### Bencode 转 JSON

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

输出:

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

#### JSON 转 Bencode

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

输出:

```text
d8:announce41:http://bttracker.debian.org:6969/announce7:comment35:"Debian CD from cdimage.debian.org"10:created by13:mktorrent 1.113:creation datei1671279452e4:infod6:lengthi3909091328e4:name29:debian-11.6.0-amd64-DVD-1.iso12:piece lengthi262144e6:pieces41:(binary blob of the hashes of each piece)ee
```

## 构建

项目需要 C++17 的支持, 同时使用了这些库: 

构建测试时:

- [google/googletest](https://github.com/google/googletest)

构建示例时:

- [neujson](https://github.com/hominsu/neujson)

在构建过程中, 所有依赖都会自动从 github 中获取, 您不需要配置它们。

使用 CMake build types, 您可以控制是否构建示例和测试。

```bash
cmake -H. -Bbuild \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=/Users/hominsu/utils/install \
	-DNEUJSON_BUILD_EXAMPLES=ON \
	-DNEUJSON_BUILD_TESTS=ON
cmake --build ./build --parallel $(nproc)
ctest -VV --test-dir ./build/ --output-on-failure
cmake --install ./build
```

或者只作为 CMake 包安装。

```bash
cmake -H. -Bbuild \
	-DNEUJSON_BUILD_EXAMPLES=OFF \
	-DNEUJSON_BUILD_TESTS=OFF
cmake --install ./build
```

卸载

```bash
cd build
make uninstall
```

## 集成

在 CMake 中使用 `find_package` 定位 bencode 库。

```cmake
find_package(bencode REQUIRED)
target_include_directories(foo PUBLIC ${bencode_INCLUDE_DIRS})
```

## 许可

在 MIT 许可下发布。更多信息请参见 `LICENSE`。
