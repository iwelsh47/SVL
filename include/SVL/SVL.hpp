#pragma once

#define __SVL_HEADER_INCLUDED__ 1

#include <cassert>
#include <cmath>
#include <cstdint>

#define __SVL_MMX__                (1 << 0)
#define __SVL_SSE__                (1 << 1)
#define __SVL_SSE2__               (1 << 2)
#define __SVL_SSE3__               (1 << 3)
#define __SVL_SSSE3__              (1 << 4)
#define __SVL_SSE4_1__             (1 << 5)
#define __SVL_SSE4_2__             (1 << 6)
#define __SVL_AVX__                (1 << 7)
#define __SVL_AVX2__               (1 << 8)
#define __SVL_FMA__                (1 << 9)
#define __SVL_AVX512F__            (1 << 10)
#define __SVL_AVX512BW__           (1 << 11)
#define __SVL_AVX512CD__           (1 << 12)
#define __SVL_AVX512DQ__           (1 << 13)
#define __SVL_AVX512ER__           (1 << 14)
#define __SVL_AVX512IFMA__         (1 << 15)
#define __SVL_AVX512PF__           (1 << 16)
#define __SVL_AVX512VL__           (1 << 17)
#define __SVL_AVX512VPOPCNTDQ__    (1 << 18)
#define __SVL_AVX512FMAPS__        (1 << 19)
#define __SVL_AVX512VNNIW__        (1 << 20)
#define __SVL_AVX512BF16__         (1 << 21)
#define __SVL_AVX512BITALG__       (1 << 22)
#define __SVL_AVX512VBMI__         (1 << 23)
#define __SVL_AVX512VBMI2__        (1 << 24)
#define __SVL_AVX512VNNI__         (1 << 25)
#define __SVL_AVX512VP2INTERSECT__ (1 << 26)

// Default to AVX2 instructions
#ifndef SVL_INSTRUCTION_SET
//  #define SVL_INSTRUCTION_SET __SVL_AVX512VPOPCNTDQ__ - 1 // avx512
  #define SVL_INSTRUCTION_SET __SVL_AVX512F__ - 1  // avx2
//  #define SVL_INSTRUCTION_SET __SVL_AVX2__ - 1     // avx
//  #define SVL_INSTRUCTION_SET __SVL_AVX__ - 1      // sse4.2
//  #define SVL_INSTRUCTION_SET __SVL_SSE4_2__ - 1   // sse4.1
//  #define SVL_INSTRUCTION_SET __SVL_SSE4_1__ - 1   // ssse3
//  #define SVL_INSTRUCTION_SET __SVL_SSSE3__ - 1    // sse3
//  #define SVL_INSTRUCTION_SET __SVL_SSE3__ - 1     // sse2
//  #define SVL_INSTRUCTION_SET __SVL_SSE2__ - 1     // sse
//  #define SVL_INSTRUCTION_SET __SVL_SSE__ - 1      // mmx
//  #define SVL_INSTRUCTION_SET __SVL_MMX__ - 1      // none
#endif

// Check which instruction sets we're using
#define __USE_MMX__    (SVL_INSTRUCTION_SET & __SVL_MMX__)
#define __USE_SSE__    (SVL_INSTRUCTION_SET & __SVL_SSE__)
#define __USE_SSE2__   (SVL_INSTRUCTION_SET & __SVL_SSE2__)
#define __USE_SSE3__   (SVL_INSTRUCTION_SET & __SVL_SSE3__)
#define __USE_SSSE3__  (SVL_INSTRUCTION_SET & __SVL_SSSE3__)
#define __USE_SSE41__  (SVL_INSTRUCTION_SET & __SVL_SSE4_1__)
#define __USE_SSE42__  (SVL_INSTRUCTION_SET & __SVL_SSE4_2__)
#define __USE_AVX__    (SVL_INSTRUCTION_SET & __SVL_AVX__)
#define __USE_AVX2__   (SVL_INSTRUCTION_SET & __SVL_AVX2__)
#define __USE_AVX512__ (SVL_INSTRUCTION_SET & __SVL_AVX512VL__)
#define __USE_FMA__    (SVL_INSTRUCTION_SET & __SVL_FMA__)

// Check for required dependencies
#if (__USE_SSE41__) && !(__USE_SSE2__)
  #error "SSE4.1 instructions require SSE2 instructions to function."
#endif
#if (__USE_SSE42__) && !(__USE_SSE2__)
  #error "SSE4.2 instructions require SSE2 instructions to function."
#endif

#if SVL_INSTRUCTION_SET
  #include <x86intrin.h>
#endif

// Some useful macros
//! Iterate over all numbers (i) from 0 to end
#define SVL_FOR_RANGE(i, end) for (auto i = decltype(end)(0); i < end; ++i)
//! Iterate over all numbers (i) from start to end
#define SVL_FOR_BEGIN_END(i, begin, end)                                       \
  for (auto i = decltype(end)(begin); i < end; ++i)
//! Iter over every step number from start to end
#define SVL_FOR_BEGIN_END_STEP(i, begin, end, step)                            \
  for (auto i = decltype(end)(begin); i < end; i += decltype(end)(step))
//! Get the maximum of two values
#define SVL_MAX(val1, val2) ((val1 > val2) ? val1 : val2)
//! Get the minimum of two values
#define SVL_MIN(val1, val2) ((val1 > val2) ? val2 : val1)
//! Clamp a value between low and high
#define SVL_CLAMP(low, val, high) SVL_MIN(SVL_MAX(low, val), high)
//! Generate a type which returns a constant float
#define SVL_CONST_FLT(val)                                                     \
  typedef struct {                                                             \
    operator float() {                                                         \
      return val;                                                              \
    }                                                                          \
  }

namespace SVL {
  // Template for blending, so can 'blend' scalar values
  template <class ScalarT>
ScalarT blend(const ScalarT& a, const ScalarT& b, bool mask) {
  return mask ? a : b;
}
}  // namespace SVL

// Load the forward declarations
#include "SVL_fwd.hpp"

// Load the implementations
#include "details/SVL_impl.hpp"

// Include the double precision floats
//#include "details/double4.hpp"
