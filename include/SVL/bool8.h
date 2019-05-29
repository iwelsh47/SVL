#ifndef __SVL_HEADER_INCLUDED__
#error Please include the SVL.h header only
#endif

struct Vector8b {
  VECTOR_BOOL_SETUP(Vector8b, 8, Vector4b);
  
#if SVL_SIMD_LEVEL < SVL_AVX2
  using intrinsic_t = struct { half_t v0_3, v4_7; };
#else
  using intrinsic_t = __m256i;
#endif
  intrinsic_t data;
  
  // Constructors
  //! Default constructor
  Vector8b() = default;
  //! Copy constructor
  Vector8b(const self_t&) = default;
  //! Move constructor
  Vector8b(self_t&&) = default;
  //! Copy assignment
  self_t& operator=(const self_t&) = default;
  //! Move assignment
  self_t& operator=(self_t&&) = default;
  //! Destructor
  ~Vector8b() = default;
  
  //! Broadcast a value to all elements
  Vector8b(bool v) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    data = { half_t(v), half_t(v) };
#else
    scalar_t V = v ? true_ : false_;
    data = _mm256_set1_epi32(V);
#endif
  }
  //! Construct from the given values
  Vector8b(bool v0, bool v1, bool v2, bool v3,
           bool v4, bool v5, bool v6, bool v7) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    data = { half_t(v0, v1, v2, v3), half_t(v4, v5, v6, v7) };
#else
    scalar_t V0 = v0 ? true_ : false_;
    scalar_t V1 = v1 ? true_ : false_;
    scalar_t V2 = v2 ? true_ : false_;
    scalar_t V3 = v3 ? true_ : false_;
    scalar_t V4 = v4 ? true_ : false_;
    scalar_t V5 = v5 ? true_ : false_;
    scalar_t V6 = v6 ? true_ : false_;
    scalar_t V7 = v7 ? true_ : false_;
    data = _mm256_setr_epi32(V0, V1, V2, V3, V4, V5, V6, V7);
#endif
  }
  //! Construct from two smaller types
  Vector8b(const half_t& a, const half_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    data = { a, b };
#else
    data = _mm256_set_m128i(b, a);
#endif
  }
  //! Convert from intrinsic type
  Vector8b(const intrinsic_t& v) : data(v) { }
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
  Vector8b(i8) = delete;
  Vector8b(i16) = delete;
  Vector8b(i32) = delete;
  Vector8b(i64) = delete;
  Vector8b(u8) = delete;
  Vector8b(u16) = delete;
  Vector8b(u32) = delete;
  Vector8b(u64) = delete;
  //! Prevent assignment from integer types
  Vector8b& operator=(i8) = delete;
  Vector8b& operator=(i16) = delete;
  Vector8b& operator=(i32) = delete;
  Vector8b& operator=(i64) = delete;
  Vector8b& operator=(u8) = delete;
  Vector8b& operator=(u16) = delete;
  Vector8b& operator=(u32) = delete;
  Vector8b& operator=(u64) = delete;
  
  // Access single value
  //! RO access to a single value
  bool access(i64 idx) const {
    idx = SVL_CLAMP(0, idx, step - 1);
    switch (idx) {
#if SVL_SIMD_LEVEL < SVL_AVX2
      case 0: case 1: case 2: case 3: return data.v0_3[idx];
      case 4: case 5: case 6: case 7: return data.v4_7[idx - step / 2];
#else
      case 0: return bool(_mm256_extract_epi32(data, 0));
      case 1: return bool(_mm256_extract_epi32(data, 1));
      case 2: return bool(_mm256_extract_epi32(data, 2));
      case 3: return bool(_mm256_extract_epi32(data, 3));
      case 4: return bool(_mm256_extract_epi32(data, 4));
      case 5: return bool(_mm256_extract_epi32(data, 5));
      case 6: return bool(_mm256_extract_epi32(data, 6));
      case 7: return bool(_mm256_extract_epi32(data, 7));
#endif
    }
    return false;  // should never reach here
  }
  //! RO access to a single value
  bool operator[](i64 idx) const { return access(idx); }
  //! Assign a single value
  self_t& assign(bool v, i64 idx) {
#if SVL_SIMD_LEVEL >= SVL_AVX2
    scalar_t V = v ? true_ : false_;
#endif
    idx = SVL_CLAMP(0, idx, step - 1);
    switch (idx) {
#if SVL_SIMD_LEVEL < SVL_AVX2
      case 0: case 1: case 2: case 3: data.v0_3.assign(v, idx); break;
      case 4: case 5: case 6: case 7: data.v4_7.assign(v, idx - step / 2); break;
#else
      case 0: data = _mm256_insert_epi32(data, V, 0); break;
      case 1: data = _mm256_insert_epi32(data, V, 1); break;
      case 2: data = _mm256_insert_epi32(data, V, 2); break;
      case 3: data = _mm256_insert_epi32(data, V, 3); break;
      case 4: data = _mm256_insert_epi32(data, V, 4); break;
      case 5: data = _mm256_insert_epi32(data, V, 5); break;
      case 6: data = _mm256_insert_epi32(data, V, 6); break;
      case 7: data = _mm256_insert_epi32(data, V, 7); break;
#endif
    }
    return *this;
  }
  
  // Logical operators
  //! Bitwise AND of two vectors
  friend inline self_t operator&(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    self_t r;
    r.data = { (a.data.v0_3 & b.data.v0_3), (a.data.v4_7 & b.data.v4_7) };
#else
    self_t r = _mm256_and_si256(a, b);
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
#if SVL_SIMD_LEVEL < SVL_AVX2
    self_t r;
    r.data = { (a.data.v0_3 | b.data.v0_3), (a.data.v4_7 | b.data.v4_7) };
#else
    self_t r = _mm256_or_si256(a, b);
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
#if SVL_SIMD_LEVEL < SVL_AVX2
    self_t r;
    r.data = { (a.data.v0_3 ^ b.data.v0_3), (a.data.v4_7 ^ b.data.v4_7) };
#else
    self_t r = _mm256_xor_si256(a, b);
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
    return a ^ self_t(true);
  }
  //! Bitwise ANDNOT of two vectors
  friend inline self_t and_not(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    self_t r;
    r.data = { and_not(a.data.v0_3, b.data.v0_3), and_not(a.data.v4_7, b.data.v4_7) };
#else
    self_t r = _mm256_andnot_si256(a, b);
#endif
    return r;
  }
  //! Inplace bitwise ANDNOT of two vectors
  friend inline self_t& and_not_inplace(self_t& a, const self_t& b) {
    a = and_not(a, b);
    return a;
  }
  
  // Horizontal working functions
  //! \todo Check if the intrinsic functions are correct
  //! Return if all values are true
  bool all() const {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return data.v0_3.all() && data.v4_7.all();
#else
    return _mm256_testc_si256(data, self_t(true));
#endif
  }
  //! Return if any values are true
  bool any() const {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return data.v0_3.any() || data.v4_7.any();
#else
    return _mm256_testnzc_si256(data, self_t(true));
#endif
  }
  //! Return if no values are true
  bool none() const { return !all(); }
};

#ifdef DEBUG
static inline std::ostream& operator<<(std::ostream& os, const Vector8b& v) {
  os << "<" << std::boolalpha;
  SVL_FOR_RANGE(v.step) os << v[i] << ((i < (v.step - 1)) ? ", " : "");
  os << ">";
  return os;
}
#endif
