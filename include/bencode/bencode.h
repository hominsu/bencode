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
// Created by Homin Su on 2023/3/8.
//

#ifndef BENCODE_INCLUDE_BENCODE_BENCODE_H_
#define BENCODE_INCLUDE_BENCODE_BENCODE_H_

#if defined(_WIN64) || defined(WIN64) || defined(_WIN32) || defined(WIN32)
#if defined(_WIN64) || defined(WIN64)
#define BENCODE_ARCH_64 1
#else
#define BENCODE_ARCH_32 1
#endif
#define BENCODE_PLATFORM_STRING "windows"
#define BENCODE_WINDOWS 1
#elif defined(__linux__)
#define BENCODE_PLATFORM_STRING "linux"
#define BENCODE_LINUX 1
#ifdef _LP64
#define BENCODE_ARCH_64 1
#else /* _LP64 */
#define BENCODE_ARCH_32 1
#endif /* _LP64 */
#elif defined(__APPLE__)
#define BENCODE_PLATFORM_STRING "osx"
#define BENCODE_APPLE 1
#ifdef _LP64
#define BENCODE_ARCH_64 1
#else /* _LP64 */
#define BENCODE_ARCH_32 1
#endif /* _LP64 */
#endif

#ifndef BENCODE_WINDOWS
#define BENCODE_WINDOWS 0
#endif
#ifndef BENCODE_LINUX
#define BENCODE_LINUX 0
#endif
#ifndef BENCODE_APPLE
#define BENCODE_APPLE 0
#endif

#if defined(__has_builtin)
#define BENCODE_HAS_BUILTIN(x) __has_builtin(x)
#else
#define BENCODE_HAS_BUILTIN(x) 0
#endif

#ifndef BENCODE_ASSERT
#include <cassert>
#define BENCODE_ASSERT(x) assert(x)
#endif // BENCODE_ASSERT

/**
 * @brief const array length
 */
#ifndef BENCODE_LENGTH
#define BENCODE_LENGTH(CONST_ARRAY) (sizeof(CONST_ARRAY) / sizeof(CONST_ARRAY[0]))
#endif // BENCODE_LENGTH

/**
 * @brief const string length
 */
#ifndef BENCODE_STR_LENGTH
#if defined(_MSC_VER)
#define BENCODE_STR_LENGTH(CONST_STR) _countof(CONST_STR)
#else
#define BENCODE_STR_LENGTH(CONST_STR) (sizeof(CONST_STR) / sizeof(CONST_STR[0]))
#endif
#endif // BENCODE_STR_LENGTH

// stringification
#define BENCODE_STRINGIFY(X) BENCODE_DO_STRINGIFY(X)
#define BENCODE_DO_STRINGIFY(X) #X

// concatenation
#define BENCODE_JOIN(X, Y) BENCODE_DO_JOIN(X, Y)
#define BENCODE_DO_JOIN(X, Y) X##Y

/**
 * @brief adopted from Boost
 */
#define BENCODE_VERSION_CODE(x,y,z) (((x)*100000) + ((y)*100) + (z))

/**
 * @brief gnuc version
 */
#if defined(__GNUC__)
#define BENCODE_GNUC \
    BENCODE_VERSION_CODE(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__)
#endif

#if defined(__clang__) || (defined(BENCODE_GNUC) && BENCODE_GNUC >= BENCODE_VERSION_CODE(4,2,0))

#define BENCODE_PRAGMA(x) _Pragma(BENCODE_STRINGIFY(x))
#if defined(__clang__)
#define BENCODE_DIAG_PRAGMA(x) BENCODE_PRAGMA(clang diagnostic x)
#else
#define BENCODE_DIAG_PRAGMA(x) BENCODE_PRAGMA(GCC diagnostic x)
#endif
#define BENCODE_DIAG_OFF(x) BENCODE_DIAG_PRAGMA(ignored BENCODE_STRINGIFY(BENCODE_JOIN(-W,x)))

// push/pop support in Clang and GCC>=4.6
#if defined(__clang__) || (defined(BENCODE_GNUC) && BENCODE_GNUC >= BENCODE_VERSION_CODE(4,6,0))
#define BENCODE_DIAG_PUSH BENCODE_DIAG_PRAGMA(push)
#define BENCODE_DIAG_POP  BENCODE_DIAG_PRAGMA(pop)
#else // GCC >= 4.2, < 4.6
#define BENCODE_DIAG_PUSH /* ignored */
#define BENCODE_DIAG_POP /* ignored */
#endif

#elif defined(_MSC_VER)

// pragma (MSVC specific)
#define BENCODE_PRAGMA(x) __pragma(x)
#define BENCODE_DIAG_PRAGMA(x) BENCODE_PRAGMA(warning(x))

#define BENCODE_DIAG_OFF(x) BENCODE_DIAG_PRAGMA(disable: x)
#define BENCODE_DIAG_PUSH BENCODE_DIAG_PRAGMA(push)
#define BENCODE_DIAG_POP  BENCODE_DIAG_PRAGMA(pop)

#else

#define BENCODE_DIAG_OFF(x) /* ignored */
#define BENCODE_DIAG_PUSH   /* ignored */
#define BENCODE_DIAG_POP    /* ignored */

#endif

/*
 * @brief Avoid compiler warnings
 */
#ifndef BENCODE_UINT64_C2
#define BENCODE_UINT64_C2(high32, low32) ((static_cast<uint64_t>(high32) << 32) | static_cast<uint64_t>(low32))
#endif

#endif //BENCODE_INCLUDE_BENCODE_BENCODE_H_
