#ifndef __SVL_HEADER_INCLUDED__
#error Please include the SVL.h header only
#endif

struct Vector4f {
  VECTOR_NUMBER_SETUP(Vector4f, 4, Vector4b, flt, std::nullptr_t);
  
#if SVL_SIMD_LEVEL < SVL_SSE
  using intrinsic_t = struct { scalar_t v0, v1, v2, v3; };
#else
  // Intrinsic type will always be _m128 with simd
  using intrinsic_t = __m128;
#endif
  intrinsic_t data;
  
  static self_t zeros() {
#if SVL_SIMD_LEVEL < SVL_SSE
    self_t r;
    memset(&r.data, 0, sizeof(r.data));
    return r;
#else
    return _mm_setzero_ps();
#endif
  }
  
  // Constructors
  //! Default constructor
  Vector4f() = default;
  //! Copy constructor
  Vector4f(const self_t&) = default;
  //! Move constructor
  Vector4f(self_t&&) = default;
  //! Copy assignment
  self_t& operator=(const self_t&) = default;
  //! Move assignment
  self_t& operator=(self_t&&) = default;
  //! Destructor
  ~Vector4f() = default;
  
  //! Construct from an array
  Vector4f(const scalar_t* arr) {
#if SVL_SIMD_LEVEL < SVL_SSE
    memcpy(&data, arr, sizeof(data));
#else
    data = _mm_loadu_ps(arr);
#endif
  }
  //! Broadcast a value to all elements
  Vector4f(scalar_t v) {
#if SVL_SIMD_LEVEL < SVL_SSE
    data = {v, v, v, v};
#else
    data = _mm_set1_ps(v);
#endif
  }
  //! Construct from the given values
  Vector4f(scalar_t v0, scalar_t v1, scalar_t v2, scalar_t v3) {
#if SVL_SIMD_LEVEL < SVL_SSE
    data = { v0, v1, v2, v3 };
#else
    data = _mm_setr_ps(v0, v1, v2, v3);
#endif
  }
  //! Convert from intrinsic type
  Vector4f(const intrinsic_t& v) : data(v) { }
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
#if SVL_SIMD_LEVEL < SVL_SSE
    memcpy(&data, arr, sizeof(data));
#else
    data = _mm_loadu_ps(arr);
#endif
    return *this;
  }
  //! Load upto step given values
  self_t& load(scalar_t v0 = 0.f, scalar_t v1 = 0.f,
               scalar_t v2 = 0.f, scalar_t v3 = 0.f) {
    *this = self_t(v0, v1, v2, v3);
    return *this;
  }
  //! Load n values from an array. Rest of data will be set to 0
  self_t& load_partial(const scalar_t* arr, i64 n) {
    n = SVL_CLAMP(0, n, step);
#if SVL_SIMD_LEVEL < SVL_SSE
    memset(&data, 0, sizeof(data));
    memcpy(&data, arr, (u64)n * sizeof(scalar_t));
#else
    switch (n) {
        // Nothing to load
      case 0: data = _mm_setzero_ps(); break;
        // Load one value
      case 1: data = _mm_load_ss(arr); break;
        // Load from arr as a double ( == 2 float)
      case 2: data = _mm_castpd_ps(_mm_load_sd((double*)arr)); break;
        // Load a double then a single from arr and combine them
      case 3:
        data = _mm_movelh_ps(_mm_castpd_ps(_mm_load_sd((double*)arr)),
                             _mm_load_ss(arr + 2)); break;
        // Normal load
      case 4: load(arr); break;
    }
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
#if SVL_SIMD_LEVEL < SVL_SSE
    memcpy(arr, &data, sizeof(data));
#else
    _mm_storeu_ps(arr, data);
#endif
  }
  //! Store n values in an array
  void store_partial(scalar_t* arr, i64 n) const {
    n = SVL_CLAMP(0, n, step);
#if SVL_SIMD_LEVEL < SVL_SSE
    memcpy(arr, &data, sizeof(scalar_t) * (u64)n);
#else
    switch (n) {
        // Nothing to store
      case 0: break;
        // Store one value
      case 1: _mm_store_ss(arr, data); break;
        // Cast to double and store one value
      case 2: _mm_store_sd((double *)arr, _mm_castps_pd(data)); break;
        // Store a double, shift down then store a single
      case 3:
        _mm_store_sd((double *)arr, _mm_castps_pd(data));
        _mm_store_ss(arr + 2, _mm_movehl_ps(data, data));
        break;
        // Normal store
      case 4: store(arr); break;
    }
#endif
  }
  
  // Access single value
  //! RO access to a single value
  //! \todo maybe faster to store in tmp array then extract from index
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
#if SVL_SIMD_LEVEL < SVL_SSE
      case 0: data.v0 = v; break;
      case 1: data.v1 = v; break;
      case 2: data.v2 = v; break;
      case 3: data.v3 = v; break;
#else
      case 0: data = _mm_blend_ps(data, _mm_set1_ps(v), 1 << 0); break;
      case 1: data = _mm_blend_ps(data, _mm_set1_ps(v), 1 << 1); break;
      case 2: data = _mm_blend_ps(data, _mm_set1_ps(v), 1 << 2); break;
      case 3: data = _mm_blend_ps(data, _mm_set1_ps(v), 1 << 3); break;
#endif
    }
    return *this;
  }
  
  // Addition operators
  //! Addition of two vectors
  friend inline self_t operator+(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(a.data.v0 + b.data.v0,
                  a.data.v1 + b.data.v1,
                  a.data.v2 + b.data.v2,
                  a.data.v3 + b.data.v3);
#else
    return _mm_add_ps(a, b);
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
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(a.data.v0 - b.data.v0,
                  a.data.v1 - b.data.v1,
                  a.data.v2 - b.data.v2,
                  a.data.v3 - b.data.v3);
#else
    return _mm_sub_ps(a, b);
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
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(a.data.v0 * b.data.v0,
                  a.data.v1 * b.data.v1,
                  a.data.v2 * b.data.v2,
                  a.data.v3 * b.data.v3);
#else
    return _mm_mul_ps(a, b);
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
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(a.data.v0 / b.data.v0,
                  a.data.v1 / b.data.v1,
                  a.data.v2 / b.data.v2,
                  a.data.v3 / b.data.v3);
#else
    return _mm_div_ps(a, b);
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
#if SVL_SIMD_LEVEL < SVL_SSE
    return bool_t(a.data.v0 == b.data.v0,
                  a.data.v1 == b.data.v1,
                  a.data.v2 == b.data.v2,
                  a.data.v3 == b.data.v3);
#else
    return _mm_castps_si128(_mm_cmpeq_ps(a, b));
#endif
  }
  //! Returns true for all elements where a != b
  friend inline bool_t operator!=(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
      return bool_t(a.data.v0 != b.data.v0,
                    a.data.v1 != b.data.v1,
                    a.data.v2 != b.data.v2,
                    a.data.v3 != b.data.v3);
#else
    return _mm_castps_si128(_mm_cmpneq_ps(a, b));
#endif
  }
  //! Returns true for all elements where a < b
  friend inline bool_t operator<(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return bool_t(a.data.v0 < b.data.v0,
                  a.data.v1 < b.data.v1,
                  a.data.v2 < b.data.v2,
                  a.data.v3 < b.data.v3);
#else
    return _mm_castps_si128(_mm_cmplt_ps(a, b));
#endif
  }
  //! Returns true for all elements where a <= b
  friend inline bool_t operator<=(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return bool_t(a.data.v0 <= b.data.v0,
                  a.data.v1 <= b.data.v1,
                  a.data.v2 <= b.data.v2,
                  a.data.v3 <= b.data.v3);
#else
    return _mm_castps_si128(_mm_cmple_ps(a, b));
#endif
  }
  //! Returns true for all elements where a > b
  friend inline bool_t operator>(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return bool_t(a.data.v0 > b.data.v0,
                  a.data.v1 > b.data.v1,
                  a.data.v2 > b.data.v2,
                  a.data.v3 > b.data.v3);
#else
    return _mm_castps_si128(_mm_cmpgt_ps(a, b));
#endif
  }
  //! Returns true for all elements where a >= b
  friend inline bool_t operator>=(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return bool_t(a.data.v0 >= b.data.v0,
                  a.data.v1 >= b.data.v1,
                  a.data.v2 >= b.data.v2,
                  a.data.v3 >= b.data.v3);
#else
    return _mm_castps_si128(_mm_cmpge_ps(a, b));
#endif
  }
  
  // General functions
  //! Blend two vectors given choice. Takes from a if c is true, b if false.
  friend inline self_t blend(const self_t& a, const self_t& b, const bool_t& c) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(c.data.v0 ? a.data.v0 : b.data.v0,
                  c.data.v1 ? a.data.v1 : b.data.v1,
                  c.data.v2 ? a.data.v2 : b.data.v2,
                  c.data.v3 ? a.data.v3 : b.data.v3);
#else
    return _mm_blendv_ps(b, a, _mm_castsi128_ps(c));
#endif
  }
  //! Returns the sum of all elements
  friend inline scalar_t horizontal_add(const self_t& a) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return a.data.v0 + a.data.v1 + a.data.v2 + a.data.v3;
#else
    intrinsic_t tmp = _mm_hadd_ps(a, a);
    return self_t(_mm_hadd_ps(tmp, tmp))[0];
#endif
  }
  //! Find the maximum elements between two vectors
  friend inline self_t max(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(SVL_MAX(a.data.v0, b.data.v0),
                  SVL_MAX(a.data.v1, b.data.v1),
                  SVL_MAX(a.data.v2, b.data.v2),
                  SVL_MAX(a.data.v3, b.data.v3));
#else
    return _mm_max_ps(a, b);
#endif
  }
  //! Find the maximum element in a vector
  friend inline scalar_t horizontal_max(const self_t& a) {
    return SVL_MAX(SVL_MAX(a[0], a[1]), SVL_MAX(a[2], a[3]));
  }
  //! Find the minimum elements between two vectors
  friend inline self_t min(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(SVL_MIN(a.data.v0, b.data.v0),
                  SVL_MIN(a.data.v1, b.data.v1),
                  SVL_MIN(a.data.v2, b.data.v2),
                  SVL_MIN(a.data.v3, b.data.v3));
#else
    return _mm_min_ps(a, b);
#endif
  }
  //! Find the minimum element in a vector
  friend inline scalar_t horizontal_min(const self_t& a) {
    return SVL_MIN(SVL_MIN(a[0], a[1]), SVL_MIN(a[2], a[3]));
  }
  
  // Special math functions
  //! Finds the square root of all elements in a
  friend inline self_t sqrt(const self_t& a) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(sqrt(a.data.v0),
                  sqrt(a.data.v1),
                  sqrt(a.data.v2),
                  sqrt(a.data.v3));
#else
    return _mm_sqrt_ps(a);
#endif
  }
  
  //! Calculates the sine of all elements in x
  friend inline self_t sin(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(sin(x.data.v0),
                  sin(x.data.v1),
                  sin(x.data.v2),
                  sin(x.data.v3));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(sin(x[i]), i);
    return r;
#endif
  }
  
  //! Calculates the cosine of all elements in x
  friend inline self_t cos(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(cos(x.data.v0),
                  cos(x.data.v1),
                  cos(x.data.v2),
                  cos(x.data.v3));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(cos(x[i]), i);
    return r;
#endif
  }
  //! Calculates the tangent of all elements in x
  friend inline self_t tan(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(tan(x.data.v0),
                  tan(x.data.v1),
                  tan(x.data.v2),
                  tan(x.data.v3));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(tan(x[i]), i);
    return r;
#endif
  }
  //! Calculates the arcsine of all elements in x
  friend inline self_t asin(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(asin(x.data.v0),
                  asin(x.data.v1),
                  asin(x.data.v2),
                  asin(x.data.v3));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(asin(x[i]), i);
    return r;
#endif
  }
  
  //! Calculates the arccosine of all elements in x
  friend inline self_t acos(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(acos(x.data.v0),
                  acos(x.data.v1),
                  acos(x.data.v2),
                  acos(x.data.v3));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(acos(x[i]), i);
    return r;
#endif
  }
  //! Calculates the arctangent of all elements in x
  friend inline self_t atan(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(atan(x.data.v0),
                  atan(x.data.v1),
                  atan(x.data.v2),
                  atan(x.data.v3));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(atan(x[i]), i);
    return r;
#endif
  }
  //! Calculates the arctangent of all elements in y/x determining the correct quadrant
  friend inline self_t atan2(const self_t& y, const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(atan2(y.data.v0, x.data.v0),
                  atan2(y.data.v1, x.data.v1),
                  atan2(y.data.v2, x.data.v2),
                  atan2(y.data.v3, x.data.v3));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(atan2(y[i], x[i]), i);
    return r;
#endif
  }
  //! Remove the sign bit from all elements in x
  friend inline self_t abs(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(fabs(x.data.v0), fabs(x.data.v1),
                  fabs(x.data.v2), fabs(x.data.v3));
#else
    __m128i mask = _mm_set_epi32(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF);
    return _mm_and_ps(_mm_castsi128_ps(mask), x.data);
#endif
  }
  
  //! Floor of the values of x
  friend inline self_t floor(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(floor(x.data.v0), floor(x.data.v1),
                  floor(x.data.v2), floor(x.data.v3));
#else
    return _mm_floor_ps(x.data);
#endif
  }
  //! Ceil of the values of x
  friend inline self_t ceil(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(ceil(x.data.v0), ceil(x.data.v1),
                  ceil(x.data.v2), ceil(x.data.v3));
#else
    return _mm_ceil_ps(x.data);
#endif
  }
  
  //! Round the values of x
  friend inline self_t round(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_SSE
    return self_t(round(x.data.v0), round(x.data.v1),
                  round(x.data.v2), round(x.data.v3));
#else
    return _mm_round_ps(x.data, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
#endif
  }
};

#ifdef DEBUG
static inline std::ostream& operator<<(std::ostream& os, const Vector4f& v) {
  os << "<";
  SVL_FOR_RANGE(v.step) os << v[i] << ((i < (v.step - 1)) ? ", " : "");
  os << ">";
  return os;
}
#endif
