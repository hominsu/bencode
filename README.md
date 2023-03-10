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
    Bencode parser/generator in C++17
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

## Description

bencode is a Bencode parser and generator for C++. It supports both SAX and DOM style API.

## Install

### clone the bencode repo

```bash
git clone https://github.com/hominsu/bencode.git
```

### Build and install

The following commands build and locally install bencode:

```bash
cd bencode
mkdir -p cmake/build
pushd cmake/build
cmake ../..
make -j
make install
popd
```

#### Build with example

The example code is part of the `bencode` repo source, which you cloned as part of the steps of the previous section, just add a cmake option:

```bash
cmake -DBENCODE_BUILD_EXAMPLES=ON ../..
```

#### Build with unit test

To build with benchmark，use `git submodule` to fetch all the data from that third party project and check out the appropriate commit first, then add the unit test CMake option:

```bash
git submodule update --init --recursive
pushd cmake/build
cmake -DBENCODE_BUILD_TESTS=ON ../..
...
```

## Uninstall

The following commands uninstall bencode:

```bash
pushd cmake/build
make uninstall
popd
```

## Usage at a glance

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

Output:

```text
d8:announce41:http://bttracker.debian.org:6969/announce7:comment35:"Debian CD from cdimage.debian.org"10:created by13:mktorrent 1.113:creation datei0e4:infod6:lengthi3909091328e4:name29:debian-11.6.0-amd64-DVD-1.iso12:piece lengthi262144e6:pieces41:(binary blob of the hashes of each piece)ee
```
