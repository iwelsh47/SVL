#ifndef __SVL_HEADER_INCLUDED__
#error Please include the SVL.h header only
#endif

struct Vector16f {
  VECTOR_NUMBER_SETUP(Vector16f, 16, Vector16b, flt, Vector8f);
  
#if SVL_SIMD_LEVEL < SVL_AVX512
  using intrinsic_t = struct { half_t v0_7, v8_f; };
#else
  #error AVX512 instruction set not currently supported
#endif
  intrinsic_t data;
  
  static self_t zeros() {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(half_t::zeros(), half_t::zeros());
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  
  // Constructors
  //! Default constructor
  Vector16f() = default;
  //! Copy constructor
  Vector16f(const self_t&) = default;
  //! Move constructor
  Vector16f(self_t&&) = default;
  //! Copy assignment
  self_t& operator=(const self_t&) = default;
  //! Move assignment
  self_t& operator=(self_t&&) = default;
  //! Destructor
  ~Vector16f() = default;
  
  //! Construct from an array
  Vector16f(scalar_t* arr) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    data = { half_t(arr), half_t(arr + half_step) };
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Broadcast a value to all elements
  Vector16f(scalar_t v) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    data = { half_t(v), half_t(v) };
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Construct from the given values
  Vector16f(scalar_t v0, scalar_t v1, scalar_t v2, scalar_t v3,
            scalar_t v4, scalar_t v5, scalar_t v6, scalar_t v7,
            scalar_t v8, scalar_t v9, scalar_t va, scalar_t vb,
            scalar_t vc, scalar_t vd, scalar_t ve, scalar_t vf) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    data = { half_t(v0, v1, v2, v3, v4, v5, v6, v7),
             half_t(v8, v9, va, vb, vc, vd, ve, vf) };
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Construct from two Vector8fs
  Vector16f(const half_t& a, const half_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    data = { a, b };
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Convert from intrinsic type
  Vector16f(const intrinsic_t& v) : data(v) { }
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
  //! Assign from scalar type
  self_t& operator=(scalar_t v) {
    *this = self_t(v);
    return *this;
  }
  
  // Load/save data
  //! Load values from an array
  self_t& load(const scalar_t* arr) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    data.v0_7.load(arr);
    data.v8_f.load(arr + half_step);
#else
    #error AVX512 instruction set not currently supported
#endif
    return *this;
  }
  
  //! Load upto step given values
  self_t& load(scalar_t v0 = 0.f, scalar_t v1 = 0.f,
               scalar_t v2 = 0.f, scalar_t v3 = 0.f,
               scalar_t v4 = 0.f, scalar_t v5 = 0.f,
               scalar_t v6 = 0.f, scalar_t v7 = 0.f,
               scalar_t v8 = 0.f, scalar_t v9 = 0.f,
               scalar_t va = 0.f, scalar_t vb = 0.f,
               scalar_t vc = 0.f, scalar_t vd = 0.f,
               scalar_t ve = 0.f, scalar_t vf = 0.f) {
    *this = self_t(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, va, vb, vc, vd, ve, vf);
    return *this;
  }
  
  //! Load n values from an array. Rest of data will be set to 0
  self_t& load_partial(const scalar_t* arr, i64 n) {
    n = SVL_CLAMP(0, n, step);
#if SVL_SIMD_LEVEL < SVL_AVX512
    data.v0_7.load_partial(arr, n);
    data.v8_f.load_partial(arr + half_step, SVL_MAX(0, n - half_step));
#else
    #error AVX512 instruction set not currently supported
#endif
    return *this;
  }
  //! Load and broadcast a value to all elements
  self_t& broadcast(scalar_t v) {
    *this = self_t(v);
    return *this;
  }
  //! Store values in an array
  void store(scalar_t* arr) const {
#if SVL_SIMD_LEVEL < SVL_AVX512
    data.v0_7.store(arr);
    data.v8_f.store(arr + half_step);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Store n values in an array
  void store_partial(scalar_t* arr, i64 n) const {
    n = SVL_CLAMP(0, n, step);
#if SVL_SIMD_LEVEL < SVL_AVX512
    if (n <= half_step) data.v0_7.store_partial(arr, n);
    else {
      data.v0_7.store(arr);
      data.v8_f.store_partial(arr + half_step, n - half_step);
    }
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  
  // Access single value
  //! RO access to a single value
  scalar_t access(i64 idx) const {
    idx = SVL_CLAMP(0, idx, step - 1);
    scalar_t tmp[step];
    store(tmp);
    return tmp[idx];
  }
  //! RO access to a single value
  scalar_t operator[](i64 idx) const { return access(idx); }
  //! Assign a single value
  self_t& assign(scalar_t v, i64 idx) {
    idx = SVL_CLAMP(0, idx, step - 1);
    switch (idx) {
#if SVL_SIMD_LEVEL < SVL_AVX512
      case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
        data.v0_7.assign(v, idx); break;
      default: data.v8_f.assign(v, idx - half_step); break;
#else
      #error AVX512 instruction set not currently supported
#endif
    }
    return *this;
  }
  
  // Addition operators
  //! Addition of two vectors
  friend inline self_t operator+(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(a.data.v0_7 + b.data.v0_7,
                  a.data.v8_f + b.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Addition of a scalar to a vector
  friend inline self_t operator+(const self_t& a, scalar_t b) {
    return a + self_t(b);
  }
  //! Addition of a vector to a scalar
  friend inline self_t operator+(scalar_t a, const self_t& b) {
    return self_t(a) + b;
  }
  //! Inplace addition of two vectors
  friend inline self_t& operator+=(self_t& a, const self_t& b) {
    a = a + b;
    return a;
  }
  //! Inplace addition of a scalar to a vector
  friend inline self_t& operator+=(self_t& a, scalar_t b) {
    a = a + b;
    return a;
  }
  //! Prefix increment
  friend inline self_t& operator++(self_t& a) {
    a += (scalar_t)1.;
    return a;
  }
  //! Postfix increment
  friend inline self_t operator++(self_t& a, int) {
    self_t r = a;
    ++a;
    return r;
  }
  
  // Subtraction operators
  //! Subtraction of two vectors
  friend inline self_t operator-(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(a.data.v0_7 - b.data.v0_7,
                  a.data.v8_f - b.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Subtraction of a scalar from a vector
  friend inline self_t operator-(const self_t& a, scalar_t b) {
    return a - self_t(b);
  }
  //! Subtraction of a vector from a scalar
  friend inline self_t operator-(scalar_t a, const self_t& b) {
    return self_t(a) - b;
  }
  //! Inplace subtraction of two vectors
  friend inline self_t& operator-=(self_t& a, const self_t& b) {
    a = a - b;
    return a;
  }
  //! Inplace subtraction of a scalar from a vector
  friend inline self_t& operator-=(self_t& a, scalar_t b) {
    a = a - b;
    return a;
  }
  //! Prefix decrement
  friend inline self_t& operator--(self_t& a) {
    a -= (scalar_t)1.;
    return a;
  }
  //! Postfix decrement
  friend inline self_t operator--(self_t& a, int) {
    self_t r = a;
    --a;
    return r;
  }
  //! Negate all elements
  friend inline self_t operator-(const self_t& a) {
    return (a * (scalar_t)-1.);
  }
  
  // Multiplication operators
  //! Multiplication of two vectors
  friend inline self_t operator*(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(a.data.v0_7 * b.data.v0_7,
                  a.data.v8_f * b.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Multiplication of a vector by a scalar
  friend inline self_t operator*(const self_t& a, scalar_t b) {
    return a * self_t(b);
  }
  //! Multiplication of a scalar by a vector
  friend inline self_t operator*(scalar_t a, const self_t& b) {
    return self_t(a) * b;
  }
  //! Inplace multiplication of two vectors
  friend inline self_t& operator*=(self_t& a, const self_t& b) {
    a = a * b;
    return a;
  }
  //! Inplace multiplication of a vector by a scalar
  friend inline self_t& operator*=(self_t& a, scalar_t b) {
    a = a * b;
    return a;
  }
  
  // Division operators
  //! Division of two vectors
  friend inline self_t operator/(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(a.data.v0_7 / b.data.v0_7,
                  a.data.v8_f / b.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Division of a vector by a scalar
  friend inline self_t operator/(const self_t& a, scalar_t b) {
    scalar_t inv = ((scalar_t)1) / b;
    return a * inv;
  }
  //! Division of a scalar by a vector
  friend inline self_t operator/(scalar_t a, const self_t& b) {
    return self_t(a) / b;
  }
  //! Inplace division of two vectors
  friend inline self_t& operator/=(self_t& a, const self_t& b) {
    a = a / b;
    return a;
  }
  //! Inplace division of a vector by a scalr
  friend inline self_t& operator/=(self_t& a, scalar_t b) {
    a = a / b;
    return a;
  }
  
  // Comparison operators
  //! Returns true for all elements where a == b
  friend inline bool_t operator==(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return bool_t(a.data.v0_7 == b.data.v0_7,
                  a.data.v8_f == b.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Returns true for all elements where a != b
  friend inline bool_t operator!=(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return bool_t(a.data.v0_7 != b.data.v0_7,
                  a.data.v8_f != b.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Returns true for all elements where a < b
  friend inline bool_t operator<(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return bool_t(a.data.v0_7 < b.data.v0_7,
                  a.data.v8_f < b.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Returns true for all elements where a <= b
  friend inline bool_t operator<=(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return bool_t(a.data.v0_7 <= b.data.v0_7,
                  a.data.v8_f <= b.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Returns true for all elements where a > b
  friend inline bool_t operator>(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return bool_t(a.data.v0_7 > b.data.v0_7,
                  a.data.v8_f > b.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Returns true for all elements where a >= b
  friend inline bool_t operator>=(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return bool_t(a.data.v0_7 >= b.data.v0_7,
                  a.data.v8_f >= b.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  
  // General functions
  //! Blend two vectors given choice. Takes from a if c is true, b if false.
  friend inline self_t blend(const self_t& a, const self_t& b, const bool_t& c) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(blend(a.data.v0_7, b.data.v0_7, c.data.v0_7),
                  blend(a.data.v8_f, b.data.v8_f, c.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Returns the sum of all elements
  friend inline scalar_t horizontal_add(const self_t& a) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return horizontal_add(a.data.v0_7) + horizontal_add(a.data.v8_f);
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Find the maximum elements between two vectors
  friend inline self_t max(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(max(a.data.v0_7, b.data.v0_7),
                  max(a.data.v8_f, b.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Find the maximum element in a vector
  friend inline scalar_t horizontal_max(const self_t& a) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return SVL_MAX(horizontal_max(a.data.v0_7),
               horizontal_max(a.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Find the minimum elements between two vectors
  friend inline self_t min(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(min(a.data.v0_7, b.data.v0_7),
                  min(a.data.v8_f, b.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Find the minimum element in a vector
  friend inline scalar_t horizontal_min(const self_t& a) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return SVL_MIN(horizontal_min(a.data.v0_7),
               horizontal_min(a.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  
  // Special math functions
  //! Finds the square root of all elements in a
  friend inline self_t sqrt(const self_t& a) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(sqrt(a.data.v0_7), sqrt(a.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  
  //! Calculates the sine of all elements in x
  friend inline self_t sin(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(sin(x.data.v0_7), sin(x.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  
  //! Calculates the cosine of all elements in x
  friend inline self_t cos(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(cos(x.data.v0_7), cos(x.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Calculates the tangent of all elements in x
  friend inline self_t tan(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(tan(x.data.v0_7), tan(x.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Calculates the arcsine of all elements in x
  friend inline self_t asin(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(asin(x.data.v0_7), asin(x.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  
  //! Calculates the arccosine of all elements in x
  friend inline self_t acos(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(acos(x.data.v0_7), acos(x.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Calculates the arctangent of all elements in x
  friend inline self_t atan(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(atan(x.data.v0_7), atan(x.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Calculates the arctangent of all elements in y/x determining the correct quadrant
  friend inline self_t atan2(const self_t& y, const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(atan2(y.data.v0_7, x.data.v0_7),
                  atan2(y.data.v8_f, x.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
  //! Remove the sign bit from all elements in x
  friend inline self_t abs(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX512
    return self_t(abs(x.data.v0_7), abs(x.data.v8_f));
#else
    #error AVX512 instruction set not currently supported
#endif
  }
};


#ifdef DEBUG
std::ostream& operator<<(std::ostream& os, const Vector16f& v) {
  os << "<";
  SVL_FOR_RANGE(v.step) os << v[i] << ((i < (v.step - 1)) ? ", " : "");
  os << ">";
  return os;
}
#endif

