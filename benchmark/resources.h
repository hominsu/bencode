// MIT License
//
// Copyright (c) 2024 HominSu
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
// Created by Homin Su on 24-5-24.
//

#ifndef BENCODE_BENCHMARK_RESOURCES_H
#define BENCODE_BENCHMARK_RESOURCES_H

#include <filesystem>

namespace resource {

static std::filesystem::path ubuntu =
    RESOURCES_DIR "/ubuntu-20.04.1-live-server-amd64.iso.torrent";
static std::filesystem::path covid =
    RESOURCES_DIR "/COVID-19-image-dataset-collection.torrent";
static std::filesystem::path camelyon17 = RESOURCES_DIR "/CAMELYON17.torrent";
static std::filesystem::path integers = RESOURCES_DIR "/integers.bencode";
static std::filesystem::path pneumonia =
    RESOURCES_DIR "/RSNA_Pneumonia_Detection_Challenge.torrent";

} // namespace resource

#endif // BENCODE_BENCHMARK_RESOURCES_H
