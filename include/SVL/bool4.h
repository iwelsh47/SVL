#ifndef __SVL_HEADER_INCLUDED__
#error Please include the SVL.h header only
#endif

#include <cstring>  // for memcpy, memset etc
#include <utility>  // for std::move

#ifdef DEBUG
#include <iostream>
#endif

struct Vector4b {
  VECTOR_BOOL_SETUP(Vector4b, 4, std::nullptr_t);
  
#if SVL_SIMD_LEVEL < SVL_SSE
  using intrinsic_t = struct { scalar_t v0, v1, v2, v3; };
#else
  // Intrinsic type will always be _m128i with simd
  using intrinsic_t = __m128i;
#endif
  intrinsic_t data;
  
  //! Compile time constant constructor
  template <bool B0, bool B1=B0, bool B2=B1, bool B3=B2>
  static inline intrinsic_t constant4b() {
    scalar_t b0 = B0 ? true_ : false_;
    scalar_t b1 = B1 ? true_ : false_;
    scalar_t b2 = B2 ? true_ : false_;
    scalar_t b3 = B3 ? true_ : false_;
#if SVL_SIMD_LEVEL < SVL_SSE
    return { b0, b1, b2, b3 };
#else
    return _mm_setr_epi32(b0, b1, b2, b3);
#endif
  }
  
  // Constructors
  //! Default constructor
  Vector4b() = default;
  //! Copy constructor
  Vector4b(const self_t&) = default;
  //! Move constructor
  Vector4b(self_t&&) = default;
  //! Copy assignment
  self_t& operator=(const self_t&) = default;
  //! Move assignment
  self_t& operator=(self_t&&) = default;
  //! Destructor
  ~Vector4b() = default;
  
  //! Broadcast a value to all elements
  Vector4b(bool v) {
    scalar_t V = v ? true_ : false_;
#if SVL_SIMD_LEVEL < SVL_SSE
    data = {V, V, V, V};
#else
    data = _mm_set1_epi32(V);
#endif
  }
  //! Construct from the given values
  Vector4b(bool v0, bool v1, bool v2, bool v3) {
    scalar_t V0 = v0 ? true_ : false_;
    scalar_t V1 = v1 ? true_ : false_;
    scalar_t V2 = v2 ? true_ : false_;
    scalar_t V3 = v3 ? true_ : false_;
#if SVL_SIMD_LEVEL < SVL_SSE
    data = { V0, V1, V2, V3 };
#else
    data = _mm_setr_epi32(V0, V1, V2, V3);
#endif
  }
  //! Convert from intrinsic type
  Vector4b(const intrinsic_t& v) : data(v) { }
  //! Covert to intrinsic type
  operator intrinsic_t() const { return data; }
  //! Assign from intrinsic type
  self_t& operator=(const intrinsic_t& v) {
    *this = self_t(v);
    return *this;
  }
  //! Move assign from intrinsic type
  self_t& operator=(intrinsic_t&& v) {
    *this = self_t(v);
    return *this;
  }
  //! Assign from bool type
  self_t& operator=(bool v) {
    *this = self_t(v);
    return *this;
  }
  
  //! Prevent construction from integer types
  Vector4b(i8) = delete;
  Vector4b(i16) = delete;
  Vector4b(i32) = delete;
  Vector4b(i64) = delete;
  Vector4b(u8) = delete;
  Vector4b(u16) = delete;
  Vector4b(u32) = delete;
  Vector4b(u64) = delete;
  //! Prevent assignment from integer types
  Vector4b& operator=(i8) = delete;
  Vector4b& operator=(i16) = delete;
  Vector4b& operator=(i32) = delete;
  Vector4b& operator=(i64) = delete;
  Vector4b& operator=(u8) = delete;
  Vector4b& operator=(u16) = delete;
  Vector4b& operator=(u32) = delete;
  Vector4b& operator=(u64) = delete;
  
  // Access single value
  //! RO access to a single value
  bool access(i64 idx) const {
    idx = CLAMP(0, idx, step - 1);
    switch (idx) {
#if SVL_SIMD_LEVEL < SVL_SSE
      case 0: return bool(data.v0);
      case 1: return bool(data.v1);
      case 2: return bool(data.v2);
      case 3: return bool(data.v3);
#else
      case 0: return bool(_mm_extract_epi32(data, 0));
      case 1: return bool(_mm_extract_epi32(data, 1));
      case 2: return bool(_mm_extract_epi32(data, 2));
      case 3: return bool(_mm_extract_epi32(data, 3));
#endif
    }
    return false;  // should never reach here
  }
  //! RO access to a single value
  bool operator[](i64 idx) const { return access(idx); }
  //! Assign a single value
  self_t& assign(bool v, i64 idx) {
    scalar_t V = v ? true_ : false_;
    idx = CLAMP(0, idx, step - 1);
    switch (idx) {
#if SVL_SIMD_LEVEL < SVL_SSE
      case 0: data.v0 = V; break;
      case 1: data.v1 = V; break;
      case 2: data.v2 = V; break;
      case 3: data.v3 = V; break;
#else
      case 0: data = _mm_insert_epi32(data, V, 0); break;
      case 1: data = _mm_insert_epi32(data, V, 1); break;
      case 2: data = _mm_insert_epi32(data, V, 2); break;
      case 3: data = _mm_insert_epi32(data, V, 3); break;
#endif
    }
    return *this;
  }
  
  // Logical operators
  //! Bitwise AND of two vectors
  friend inline self_t operator&(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    self_t r;
    FOR_RANGE(step) r.assign(a[i] & b[i], i);
#else
    self_t r = _mm_and_si128(a, b);
#endif
    return r;
  }
  //! Inplace bitwise AND of two vectors
  friend inline self_t& operator&=(self_t& a, const self_t& b) {
    a = a & b;
    return a;
  }
  //! Bitwise OR of two vectors
  friend inline self_t operator|(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    self_t r;
    FOR_RANGE(step) r.assign(a[i] | b[i], i);
#else
    self_t r = _mm_or_si128(a, b);
#endif
    return r;
  }
  //! Inplace bitwise OR of two vectors
  friend inline self_t& operator|=(self_t& a, const self_t& b) {
    a = a | b;
    return a;
  }
  //! Bitwise XOR of two vectors
  friend inline self_t operator^(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    self_t r;
    FOR_RANGE(step) r.assign(a[i] ^ b[i], i);
#else
    self_t r = _mm_xor_si128(a, b);
#endif
    return r;
  }
  //! Inplace bitwise XOR of two vectors
  friend inline self_t& operator^=(self_t& a, const self_t& b) {
    a = a ^ b;
    return a;
  }
  //! Bitwise NOT of a vector
  friend inline self_t operator~(const self_t& a) {
    return a ^ constant4b<true>();
  }
  //! Bitwise ANDNOT of two vectors
  friend inline self_t and_not(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    self_t r = a & ~b;
#else
    self_t r = _mm_andnot_si128(a, b);
#endif
    return r;
  }
  //! Inplace bitwise ANDNOT of two vectors
  friend inline self_t& and_not_inplace(self_t& a, const self_t& b) {
    a = and_not(a, b);
    return a;
  }
  
  // Horizontal working functions
  //! Return if all values are true
  bool all() const {
#if SVL_SIMD_LEVEL < SVL_SSE
    FOR_RANGE(step) if (!(*this)[i]) return false;
    return true;
#else
    return _mm_test_all_ones(data);
#endif
  }
  //! Return if any values are true
  bool any() const {
#if SVL_SIMD_LEVEL < SVL_SSE
    FOR_RANGE(step) if ((*this)[i]) return true;
    return false;
#else
    return _mm_test_mix_ones_zeros(data, self_t(true));
#endif
  }
  //! Return if no values are true
  bool none() const { return !all(); }
};

#ifdef DEBUG
std::ostream& operator<<(std::ostream& os, const Vector4b& v) {
  os << "<" << std::boolalpha;
  FOR_RANGE(v.step) os << v[i] << ((i < (v.step - 1)) ? ", " : "");
  os << ">";
  return os;
}
#endif
