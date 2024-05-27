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
// Created by Homing So on 24-5-24.
//

#include <filesystem>
#include <fstream>
#include <string>

#include <benchmark/benchmark.h>

#include "jimporter_bencode.h"

#include "resources.h"

static void BM_decode_value(benchmark::State &state,
                            const std::filesystem::path &path) {
  auto ifs = std::ifstream(path, std::ifstream::binary);
  std::string torrent(std::istreambuf_iterator<char>{ifs},
                      std::istreambuf_iterator<char>{});

  for (auto _ : state) {
    benchmark::DoNotOptimize(bencode::decode(torrent));
    benchmark::ClobberMemory();
  }
  state.SetBytesProcessed(state.iterations() * torrent.size());
}

// static void BM_decode_view(benchmark::State &state,
//                            const std::filesystem::path &path) {
//   auto ifs = std::ifstream(path, std::ifstream::binary);
//   std::string torrent(std::istreambuf_iterator<char>{ifs},
//                       std::istreambuf_iterator<char>{});
//
//   for (auto _ : state) {
//     benchmark::DoNotOptimize(bencode::decode_view(torrent));
//     benchmark::ClobberMemory();
//   }
//   state.SetBytesProcessed(state.iterations() * torrent.size());
// }

BENCHMARK_CAPTURE(BM_decode_value, "ubuntu", resource::ubuntu);
BENCHMARK_CAPTURE(BM_decode_value, "covid", resource::covid);
BENCHMARK_CAPTURE(BM_decode_value, "camelyon17", resource::camelyon17);
BENCHMARK_CAPTURE(BM_decode_value, "pneumonia", resource::pneumonia);
BENCHMARK_CAPTURE(BM_decode_value, "integers", resource::integers);

// BENCHMARK_CAPTURE(BM_decode_view, "ubuntu", resource::ubuntu);
// BENCHMARK_CAPTURE(BM_decode_view, "covid", resource::covid);
// BENCHMARK_CAPTURE(BM_decode_view, "camelyon17", resource::camelyon17);
// BENCHMARK_CAPTURE(BM_decode_view, "pneumonia", resource::pneumonia);
// BENCHMARK_CAPTURE(BM_decode_view, "integers", resource::integers);

BENCHMARK_MAIN();
