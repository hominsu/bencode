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
// Created by Homin Su on 24-5-25.
//

#ifndef BENCODE_INCLUDE_BENCODE_CONCEPT_H
#define BENCODE_INCLUDE_BENCODE_CONCEPT_H

#include <concepts>

namespace bencode::concepts {

template <typename T>
concept PositiveNumber = requires(T n) {
  { n > 0 } -> std::convertible_to<bool>;
};

template <typename T>
concept GreaterEqualZeroNumber = requires(T n) {
  { n >= 0 } -> std::convertible_to<bool>;
};

template <typename Stream>
concept StreamCharTypeIsChar =
    requires { std::same_as<typename Stream::char_type, char>; };

} // namespace bencode::concepts

#endif // BENCODE_INCLUDE_BENCODE_CONCEPT_H
