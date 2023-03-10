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
    JSON parser/generator in C++17
    <br/>
    <a href="https://me.hauhau.cn/projects/bencode/"><strong>Explore the docs »</strong></a>
    <br/>
    <br/>
    <a href="https://github.com/hominsu/bencode">View Demo</a>
    ·
    <a href="https://github.com/hominsu/bencode/issues">Report Bug</a>
    ·
    <a href="https://github.com/hominsu/bencode/issues">Request Feature</a>
  </p>
</div>

##### Translations: [English](README.md) | [简体中文](README_zh.md)

## 简介

bencode 是一个 C++ 的 JSON 解析器及生成器。它同时支持 SAX 和 DOM 风格的 API。

## 安装

### 克隆 bencode 的源代码

```bash
git clone https://github.com/hominsu/bencode.git
```

### 编译和安装

下面的命令将在本地构建和安装 bencode：

```bash
cd bencode
mkdir -p cmake/build
pushd cmake/build
cmake ../..
make -j
make install
popd
```

#### 同时构建示例程序

示例的源代码已经包含在 `bencode` 的源代码中，你已经在上一步中克隆了源代码，只需要额外添加一个的 `cmake` 选项：

```bash
cmake -DBENCODE_BUILD_EXAMPLES=ON ../..
```

#### 同时构建单元测试

构建单元测试，需要使用 `git submodule` 拉取第三方依赖，并检出到相应的分支，然后添加单元测试的 `cmake` 选项：

```bash
git submodule update --init --recursive
pushd cmake/build
cmake -DBENCODE_BUILD_TESTS=ON ../..
...
```

## 卸载

使用以下命令可以卸载 bencode：

```bash
pushd cmake/build
make uninstall
popd
```

## 用法一目了然

此简单例子解析一个 Bencode 字符串到一个 document (DOM) 对象，对 DOM 作出简单修改，最终把 DOM 转换到 Bencode 字符串。

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
    puts(bencode::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  // 2. Modify it by DOM.
  auto &s = doc["creation date"];
  s.SetInteger(0);

  // 3. Stringify the DOM
  bencode::StringWriteStream os;
  bencode::Writer<bencode::StringWriteStream> writer(os);
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