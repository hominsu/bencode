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

#include <cstdint>

#include <limits>

#include "bencode/internal/itoa.h"

#include "gtest/gtest.h"

#if defined(__GNUC__) && !defined(__clang__)
BENCODE_DIAG_PUSH
BENCODE_DIAG_OFF(stringop-overflow)
#endif

template <typename T> struct Traits {};

template <> struct Traits<uint32_t> {
  enum { kBufferSize = 11 };
  enum { kMaxDigit = 10 };
  static uint32_t Negate(const uint32_t x) { return x; }
};

template <> struct Traits<int32_t> {
  enum { kBufferSize = 12 };
  enum { kMaxDigit = 10 };
  static int32_t Negate(const int32_t x) { return -x; }
};

template <> struct Traits<uint64_t> {
  enum { kBufferSize = 21 };
  enum { kMaxDigit = 20 };
  static uint64_t Negate(const uint64_t x) { return x; }
};

template <> struct Traits<int64_t> {
  enum { kBufferSize = 22 };
  enum { kMaxDigit = 20 };
  static int64_t Negate(const int64_t x) { return -x; }
};

template <typename T>
static void VerifyValue(T value, void (*f)(T, char *), char *(*g)(T, char *)) {
  char buffer1[Traits<T>::kBufferSize];
  char buffer2[Traits<T>::kBufferSize];

  f(value, buffer1);
  *g(value, buffer2) = '\0';

  EXPECT_STREQ(buffer1, buffer2);
}

template <typename T>
static void Verify(void (*f)(T, char *), char *(*g)(T, char *)) {
  // Boundary cases
  VerifyValue<T>(0, f, g);
  VerifyValue<T>((std::numeric_limits<T>::min)(), f, g);
  VerifyValue<T>((std::numeric_limits<T>::max)(), f, g);

  // 2^n - 1, 2^n, 10^n - 1, 10^n until overflow
  for (int power = 2; power <= 10; power += 8) {
    T i = 1, last;
    do {
      VerifyValue<T>(i - 1, f, g);
      VerifyValue<T>(i, f, g);
      if ((std::numeric_limits<T>::min)() < 0) {
        VerifyValue<T>(Traits<T>::Negate(i), f, g);
        VerifyValue<T>(Traits<T>::Negate(i + 1), f, g);
      }
      last = i;
      if (i > static_cast<T>((std::numeric_limits<T>::max)() /
                             static_cast<T>(power)))
        break;
      i *= static_cast<T>(power);
    } while (last < i);
  }
}

static void u32toa_naive(uint32_t value, char *buffer) {
  char temp[10];
  char *p = temp;
  do {
    *p++ = static_cast<char>(static_cast<char>(value % 10) + '0');
    value /= 10;
  } while (value > 0);

  do {
    *buffer++ = *--p;
  } while (p != temp);

  *buffer = '\0';
}

static void i32toa_naive(const int32_t value, char *buffer) {
  auto u = static_cast<uint32_t>(value);
  if (value < 0) {
    *buffer++ = '-';
    u = ~u + 1;
  }
  u32toa_naive(u, buffer);
}

static void u64toa_naive(uint64_t value, char *buffer) {
  char temp[20];
  char *p = temp;
  do {
    *p++ = static_cast<char>(static_cast<char>(value % 10) + '0');
    value /= 10;
  } while (value > 0);

  do {
    *buffer++ = *--p;
  } while (p != temp);

  *buffer = '\0';
}

static void i64toa_naive(const int64_t value, char *buffer) {
  auto u = static_cast<uint64_t>(value);
  if (value < 0) {
    *buffer++ = '-';
    u = ~u + 1;
  }
  u64toa_naive(u, buffer);
}

TEST(itoa, u32toa) { Verify(u32toa_naive, bencode::internal::u32toa); }

TEST(itoa, i32toa) { Verify(i32toa_naive, bencode::internal::i32toa); }

TEST(itoa, u64toa) { Verify(u64toa_naive, bencode::internal::u64toa); }

TEST(itoa, i64toa) { Verify(i64toa_naive, bencode::internal::i64toa); }

#if defined(__GNUC__) && !defined(__clang__)
BENCODE_DIAG_POP
#endif
