// Forward declare the vector classes that are available
namespace SVL {
  // Scalar namespace is always available
  namespace scalar {
    struct Vector4b;
    struct Vector8b;
    struct Vector16b;
    struct Vector4i;
    struct Vector8i;
    struct Vector16i;
    struct Vector4f;
    struct Vector8f;
    struct Vector16f;
    struct Vector4d;
    struct Vector8d;
    struct Vector16d;
  }
  
#if SVL_USE_SSE || SVL_USE_AVX2 || SVL_USE_AVX512
  namespace sse {
    struct Vector4b;
    struct Vector8b;
    struct Vector16b;
    struct Vector4i;
    struct Vector8i;
    struct Vector16i;
    struct Vector4f;
    struct Vector8f;
    struct Vector16f;
    struct Vector4d;
    struct Vector8d;
    struct Vector16d;
  }
#endif
  
#if SVL_USE_AVX2 || SVL_USE_AVX512
  namespace avx2 {
    struct Vector4b;
    struct Vector8b;
    struct Vector16b;
    struct Vector4i;
    struct Vector8i;
    struct Vector16i;
    struct Vector4f;
    struct Vector8f;
    struct Vector16f;
    struct Vector4d;
    struct Vector8d;
    struct Vector16d;
  }
#endif
  
#if SVL_USE_AVX512
  namespace avx512 {
    struct Vector4b;
    struct Vector8b;
    struct Vector16b;
    struct Vector4i;
    struct Vector8i;
    struct Vector16i;
    struct Vector4f;
    struct Vector8f;
    struct Vector16f;
    struct Vector4d;
    struct Vector8d;
    struct Vector16d;
  }
#endif
  
  // Define the default SVL namespace vectors to use
#if SVL_USE_AVX512
  using Vec4f  = avx512::Vector4f;
  using Vec8f  = avx512::Vector8f;
  using Vec16f = avx512::Vector16f;
  using Vec4b  = avx512::Vector4b;
  using Vec8b  = avx512::Vector8b;
  using Vec16b = avx512::Vector16b;
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

