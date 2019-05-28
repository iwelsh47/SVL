#ifndef __SVL_HEADER_INCLUDED__
#error Please include the SVL.h header only
#endif

struct Vector16b {
  VECTOR_BOOL_SETUP(Vector16b, 16, Vector8b);
  
#if SVL_SIMD_LEVEL < SVL_AVX512
  using intrinsic_t = struct { half_t v0_7, v8_f; } ;
#else
//  using intrinsic_t = __m512i;
  #error AVX512 instruction set not currently supported
#endif
  intrinsic_t data;
  
  // Constructors
  //! Default constructor
  Vector16b() = default;
  //! Copy constructor
  Vector16b(const self_t&) = default;
  //! Move constructor
  Vector16b(self_t&&) = default;
  //! Copy assignment
  self_t& operator=(const self_t&) = default;
  //! Move assignment
  self_t& operator=(self_t&&) = default;
  //! Destructor
  ~Vector16b() = default;
  
  //! Broadcast a value to all elements
  Vector16b(bool v) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    data = {half_t(v), half_t(v)};
#else
//    scalar_t V = v ? true_ : false_;
//    data = _mm512_set1_epi32(V);
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Construct from the given values
  Vector16b(bool v0, bool v1, bool v2, bool v3,
            bool v4, bool v5, bool v6, bool v7,
            bool v8, bool v9, bool va, bool vb,
            bool vc, bool vd, bool ve, bool vf) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    data = { half_t(v0, v1, v2, v3, v4, v5, v6, v7),
             half_t(v8, v9, va, vb, vc, vd, ve, vf) };
#else
//    scalar_t V0 = v0 ? true_ : false_;
//    scalar_t V1 = v1 ? true_ : false_;
//    scalar_t V2 = v2 ? true_ : false_;
//    scalar_t V3 = v3 ? true_ : false_;
//    scalar_t V4 = v4 ? true_ : false_;
//    scalar_t V5 = v5 ? true_ : false_;
//    scalar_t V6 = v6 ? true_ : false_;
//    scalar_t V7 = v7 ? true_ : false_;
//    scalar_t V8 = v8 ? true_ : false_;
//    scalar_t V9 = v9 ? true_ : false_;
//    scalar_t Va = va ? true_ : false_;
//    scalar_t Vb = vb ? true_ : false_;
//    scalar_t Vc = vc ? true_ : false_;
//    scalar_t Vd = vd ? true_ : false_;
//    scalar_t Ve = ve ? true_ : false_;
//    scalar_t Vf = vf ? true_ : false_;
//    data = _mm512_setr_epi32(V0, V1, V2, V3, V4, V5, V6, V7,
//                             V8, V9, Va, Vb, Vc, Vd, Ve, Vf);
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Construct from two smaller
  Vector16b(const half_t& a, const half_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    data = { a, b };
#else
#error AVX512 instruction set not currently supported
#endif
  }
  //! Convert from intrinsic type
  Vector16b(const intrinsic_t& v) : data(v) { }
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
  Vector16b(i8) = delete;
  Vector16b(i16) = delete;
  Vector16b(i32) = delete;
  Vector16b(i64) = delete;
  Vector16b(u8) = delete;
  Vector16b(u16) = delete;
  Vector16b(u32) = delete;
  Vector16b(u64) = delete;
  //! Prevent assignment from integer types
  Vector16b& operator=(i8) = delete;
  Vector16b& operator=(i16) = delete;
  Vector16b& operator=(i32) = delete;
  Vector16b& operator=(i64) = delete;
  Vector16b& operator=(u8) = delete;
  Vector16b& operator=(u16) = delete;
  Vector16b& operator=(u32) = delete;
  Vector16b& operator=(u64) = delete;
  
  // Access single value
  //! RO access to a single value
  bool access(i64 idx) const {
    idx = SVL_CLAMP(0, idx, step - 1);
    switch (idx) {
#if SVL_SIMD_LEVEL < SVL_AVX512
      case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
        return data.v0_7[idx];
      case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
        return data.v8_f[idx - step / 2];
#else
      #error AVX512 instruction set not currently supported
#endif
    }
    return false;  // should never reach here
  }
  //! RO access to a single value
  bool operator[](i64 idx) const { return access(idx); }
  //! Assign a single value
  self_t& assign(bool v, i64 idx) {
//    scalar_t V = v ? true_ : false_;
    idx = SVL_CLAMP(0, idx, step - 1);
    switch (idx) {
#if SVL_SIMD_LEVEL < SVL_AVX512
      case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
        data.v0_7.assign(v, idx); break;
      case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
        data.v8_f.assign(v, idx - step / 2); break;
#else
    #error AVX512 instruction set not currently supported
#endif
    }
    return *this;
  }
  
  // Logical operators
  //! Bitwise AND of two vectors
  friend inline self_t operator&(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    self_t r;
    r.data = { (a.data.v0_7 & b.data.v0_7), (a.data.v8_f & b.data.v8_f) };
#else
  #error AVX512 instruction set not currently supported
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
#if SVL_SIMD_LEVEL < SVL_AVX512
    self_t r;
    r.data = { (a.data.v0_7 | b.data.v0_7), (a.data.v8_f | b.data.v8_f) };
#else
#error AVX512 instruction set not currently supported
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
#if SVL_SIMD_LEVEL < SVL_AVX512
    self_t r;
    r.data = { (a.data.v0_7 ^ b.data.v0_7), (a.data.v8_f ^ b.data.v8_f) };
#else
#error AVX512 instruction set not currently supported
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
#if SVL_SIMD_LEVEL < SVL_AVX512
    self_t r;
    r.data = { and_not(a.data.v0_7, b.data.v0_7),
               and_not(a.data.v8_f, b.data.v8_f) };
#else
#error AVX512 instruction set not currently supported
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
#if SVL_SIMD_LEVEL < SVL_AVX512
    return data.v0_7.all() && data.v8_f.all();
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Return if any values are true
  bool any() const {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return data.v0_7.any() | data.v8_f.any();
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Return if no values are true
  bool none() const { return !all(); }
};

#ifdef DEBUG
std::ostream& operator<<(std::ostream& os, const Vector16b& v) {
  os << "<" << std::boolalpha;
  SVL_FOR_RANGE(v.step) os << v[i] << ((i < (v.step - 1)) ? ", " : "");
  os << ">";
  return os;
}
#endif
