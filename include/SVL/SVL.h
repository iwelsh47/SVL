#pragma once

#define __SVL_HEADER_INCLUDED__ 1

// Set some integer defines
#include <cstdint>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using flt = float;
using dbl = double;

// Some useful macros
//! Performs compile time check that a constant value is a power of two
#define POWER_TWO_CHECK(val) static_assert(!(val & (val - 1)) && val > 0, "#val must be a power of two")
//! Iterate over all numbers (i) from 0 to end
#define FOR_RANGE(end) for (i64 i = 0; i < end; ++i)
//! Iterate over all numbers (i) from start to end
#define FOR_BEGIN_END(begin, end) for (i64 i = begin; i < end; ++i)
//! Get the maximum of two values
#define MAX(val1, val2) ((val1 > val2) ? val1 : val2)
//! Get the minimum of two values
#define MIN(val1, val2) ((val1 > val2) ? val2 : val1)
//! Clamp a val between low and high
#define CLAMP(low, val, high) MIN(MAX(low, val), high)

//! Sets up the commoon header space for vector bool types
#define VECTOR_BOOL_SETUP(type_name, sz, partial) \
using scalar_t = u32;\
static const i64 step = sz; \
static const scalar_t true_ = 0xFFFFFFFF; \
static const scalar_t false_ = 0; \
using self_t = type_name; \
using half_t = partial;

//! Sets up the common header space for vector number types
#define VECTOR_NUMBER_SETUP(type_name, sz, bool_type, value_t, partial) \
using scalar_t = value_t; \
static const i64 step = sz; \
static const i64 half_step = step / 2; \
using self_t = type_name; \
using bool_t = bool_type; \
using half_t = partial;


// Set the level of SIMD to use
#define SVL_NONE 0
#define SVL_SSE 1
#define SVL_AVX2 2
#define SVL_AVX512 3


// Include the scalar versions always
#include <cmath>
namespace SVL::scalar {
#define SVL_SIMD_LEVEL SVL_NONE
#include "vectors.h"
#undef SVL_SIMD_LEVEL
}

// Include the sse versions if sse, avx2 or avx512 is set
#if SVL_USE_SSE || SVL_USE_AVX2 || SVL_USE_AVX512
#include <x86intrin.h>
namespace SVL::sse {
#define SVL_SIMD_LEVEL SVL_SSE
#include "vectors.h"
#undef SVL_SIMD_LEVEL
}
#endif

// Include the avx2 versions if avx2 or avx512 is set
#if SVL_USE_AVX2 || SVL_USE_AVX512
namespace SVL::avx2 {
#define SVL_SIMD_LEVEL SVL_AVX2
#include "vectors.h"
#undef SVL_SIMD_LEVEL
}
#endif

// Include the avx512 versions if  avx512 is set
//#if SVL_USE_AVX512
//namespace SVL::avx512 {
//#define SVL_SIMD_LEVEL SVL_AVX512
//#include "vectors.h"
//#undef SVL_SIMD_LEVEL
//}
//#endif

// Define default vector types based on highest used instruction set
namespace SVL {
#if SVL_USE_AVX512
#elif SVL_USE_AVX2
  using Vec4f  = avx2::Vector4f;
  using Vec8f  = avx2::Vector8f;
  using Vec16f = avx2::Vector16f;
  using Vec4b  = avx2::Vector4b;
  using Vec8b  = avx2::Vector8b;
  using Vec16b = avx2::Vector16b;
#elif SVL_USE_SSE
  using Vec4f  = sse::Vector4f;
  using Vec8f  = sse::Vector8f;
  using Vec16f = sse::Vector16f;
  using Vec4b  = sse::Vector4b;
  using Vec8b  = sse::Vector8b;
  using Vec16b = sse::Vector16b;
#else
  using Vec4f  = scalar::Vector4f;
  using Vec8f  = scalar::Vector8f;
  using Vec16f = scalar::Vector16f;
  using Vec4b  = scalar::Vector4b;
  using Vec8b  = scalar::Vector8b;
  using Vec16b = scalar::Vector16b;
#endif
}
