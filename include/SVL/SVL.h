#pragma once

#define __SVL_HEADER_INCLUDED__ 1

#include <cmath>
#include <cstdint>
#include <cstring>  // for memcpy, memset etc
#include <utility>  // for std::move
#ifdef DEBUG
#include <iostream>
#endif

// Set some integer defines
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
//! Iterate over all numbers (i) from 0 to end
#define SVL_FOR_RANGE(end) for (i64 i = 0; i < end; ++i)
//! Iterate over all numbers (i) from start to end
#define SVL_FOR_BEGIN_END(begin, end) for (i64 i = begin; i < end; ++i)
//! Get the maximum of two values
#define SVL_MAX(val1, val2) ((val1 > val2) ? val1 : val2)
//! Get the minimum of two values
#define SVL_MIN(val1, val2) ((val1 > val2) ? val2 : val1)
//! Clamp a val between low and high
#define SVL_CLAMP(low, val, high) SVL_MIN(SVL_MAX(low, val), high)

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

// Load the forward declarations
#include "SVL_fwd.h"

// Set the level of SIMD to use
#define SVL_NONE 0
#define SVL_SSE 1
#define SVL_AVX2 2
#define SVL_AVX512 3

// Include the scalar versions always
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

