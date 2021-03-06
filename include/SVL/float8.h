#ifndef __SVL_HEADER_INCLUDED__
#error Please include the SVL.h header only
#endif

struct Vector8f {
  VECTOR_NUMBER_SETUP(Vector8f, 8, Vector8b, flt, Vector4f);
  
#if SVL_SIMD_LEVEL < SVL_AVX2
  using intrinsic_t = struct { half_t v0_3, v4_7; };
#else
  using intrinsic_t = __m256;
#endif
  intrinsic_t data;
  
  static self_t zeros() {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(half_t::zeros(), half_t::zeros());
#else
    return _mm256_setzero_ps();
#endif
  }
  
  // Constructors
  //! Default constructor
  Vector8f() = default;
  //! Copy constructor
  Vector8f(const self_t&) = default;
  //! Move constructor
  Vector8f(self_t&&) = default;
  //! Copy assignment
  self_t& operator=(const self_t&) = default;
  //! Move assignment
  self_t& operator=(self_t&&) = default;
  //! Destructor
  ~Vector8f() = default;
  
  //! Construct from an array
  Vector8f(const scalar_t* arr) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    data = { half_t(arr), half_t(arr + half_step) };
#else
    data = _mm256_loadu_ps(arr);
#endif
  }
  //! Broadcast a value to all elements
  Vector8f(scalar_t v) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    data = { half_t(v), half_t(v) };
#else
    data = _mm256_set1_ps(v);
#endif
  }
  //! Construct from the given values
  Vector8f(scalar_t v0, scalar_t v1, scalar_t v2, scalar_t v3,
           scalar_t v4, scalar_t v5, scalar_t v6, scalar_t v7) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    data = { half_t(v0, v1, v2, v3), half_t(v4, v5, v6, v7) };
#else
    data = _mm256_setr_ps(v0, v1, v2, v3, v4, v5, v6, v7);
#endif
  }
  //! Construct from two Vector4fs
  Vector8f(const half_t& a, const half_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    data = { a, b };
#else
    data = _mm256_set_m128(b, a);
#endif
  }
  //! Convert from intrinsic type
  Vector8f(const intrinsic_t& v) : data(v) { }
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
#if SVL_SIMD_LEVEL < SVL_AVX2
    data.v0_3.load(arr);
    data.v4_7.load(arr + half_step);
#else
    data = _mm256_loadu_ps(arr);
#endif
    return *this;
  }
  
  //! Load upto step given values
  self_t& load(scalar_t v0 = 0.f, scalar_t v1 = 0.f,
               scalar_t v2 = 0.f, scalar_t v3 = 0.f,
               scalar_t v4 = 0.f, scalar_t v5 = 0.f,
               scalar_t v6 = 0.f, scalar_t v7 = 0.f) {
    *this = self_t(v0, v1, v2, v3, v4, v5, v6, v7);
    return *this;
  }
  
  //! Load n values from an array. Rest of data will be set to 0
  self_t& load_partial(const scalar_t* arr, i64 n) {
    n = SVL_CLAMP(0, n, step);
#if SVL_SIMD_LEVEL < SVL_AVX2
    data.v0_3.load_partial(arr, n);
    data.v4_7.load_partial(arr + half_step, SVL_MAX(0, n - half_step));
#else
    if (n == 0) *this = zeros();
    else if (n <= half_step)
      *this = self_t(half_t().load_partial(arr, n), half_t::zeros());
    else if (n < step)
      *this = self_t(half_t(arr), half_t().load_partial(arr + half_step, n - half_step));
    else load(arr);
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
#if SVL_SIMD_LEVEL < SVL_AVX2
    data.v0_3.store(arr);
    data.v4_7.store(arr + half_step);
#else
    _mm256_storeu_ps(arr, data);
#endif
  }
  //! Store n values in an array
  void store_partial(scalar_t* arr, i64 n) const {
    n = SVL_CLAMP(0, n, step);
#if SVL_SIMD_LEVEL < SVL_AVX2
    if (n <= half_step) data.v0_3.store_partial(arr, n);
    else {
      data.v0_3.store(arr);
      data.v4_7.store_partial(arr + half_step, n - half_step);
    }
#else
    if (n <= half_step)
      half_t(_mm256_extractf128_ps(data, 0)).store_partial(arr, n);
    else {
      half_t(_mm256_extractf128_ps(data, 0)).store(arr);
      half_t(_mm256_extractf128_ps(data, 1)).store_partial(arr + half_step, n - half_step);
    }
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
#if SVL_SIMD_LEVEL < SVL_AVX2
      case 0: case 1: case 2: case 3: data.v0_3.assign(v, idx); break;
      case 4: case 5: case 6: case 7: data.v4_7.assign(v, idx - half_step); break;
#else
      case 0: data = _mm256_blend_ps(data, _mm256_set1_ps(v), 1 << 0); break;
      case 1: data = _mm256_blend_ps(data, _mm256_set1_ps(v), 1 << 1); break;
      case 2: data = _mm256_blend_ps(data, _mm256_set1_ps(v), 1 << 2); break;
      case 3: data = _mm256_blend_ps(data, _mm256_set1_ps(v), 1 << 3); break;
      case 4: data = _mm256_blend_ps(data, _mm256_set1_ps(v), 1 << 4); break;
      case 5: data = _mm256_blend_ps(data, _mm256_set1_ps(v), 1 << 5); break;
      case 6: data = _mm256_blend_ps(data, _mm256_set1_ps(v), 1 << 6); break;
      case 7: data = _mm256_blend_ps(data, _mm256_set1_ps(v), 1 << 7); break;
#endif
    }
    return *this;
  }
  
  // Addition operators
  //! Addition of two vectors
  friend inline self_t operator+(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(a.data.v0_3 + b.data.v0_3,
                  a.data.v4_7 + b.data.v4_7);
#else
    return _mm256_add_ps(a, b);
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
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(a.data.v0_3 - b.data.v0_3,
                  a.data.v4_7 - b.data.v4_7);
#else
    return _mm256_sub_ps(a, b);
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
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(a.data.v0_3 * b.data.v0_3,
                  a.data.v4_7 * b.data.v4_7);
#else
    return _mm256_mul_ps(a, b);
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
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(a.data.v0_3 / b.data.v0_3,
                  a.data.v4_7 / b.data.v4_7);
#else
    return _mm256_div_ps(a, b);
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
#if SVL_SIMD_LEVEL < SVL_AVX2
    return bool_t(a.data.v0_3 == b.data.v0_3,
                  a.data.v4_7 == b.data.v4_7);
#else
    return _mm256_castps_si256(_mm256_cmp_ps(a, b, 0));
#endif
  }
  //! Returns true for all elements where a != b
  friend inline bool_t operator!=(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return bool_t(a.data.v0_3 != b.data.v0_3,
                  a.data.v4_7 != b.data.v4_7);
#else
    return _mm256_castps_si256(_mm256_cmp_ps(a, b, 4));
#endif
  }
  //! Returns true for all elements where a < b
  friend inline bool_t operator<(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return bool_t(a.data.v0_3 < b.data.v0_3,
                  a.data.v4_7 < b.data.v4_7);
#else
    return _mm256_castps_si256(_mm256_cmp_ps(a, b, 1));
#endif
  }
  //! Returns true for all elements where a <= b
  friend inline bool_t operator<=(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return bool_t(a.data.v0_3 <= b.data.v0_3,
                  a.data.v4_7 <= b.data.v4_7);
#else
    return _mm256_castps_si256(_mm256_cmp_ps(a, b, 2));
#endif
  }
  //! Returns true for all elements where a > b
  friend inline bool_t operator>(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return bool_t(a.data.v0_3 > b.data.v0_3,
                  a.data.v4_7 > b.data.v4_7);
#else
    return _mm256_castps_si256(_mm256_cmp_ps(a, b, 14));
#endif
  }
  //! Returns true for all elements where a >= b
  friend inline bool_t operator>=(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return bool_t(a.data.v0_3 >= b.data.v0_3,
                  a.data.v4_7 >= b.data.v4_7);
#else
    return _mm256_castps_si256(_mm256_cmp_ps(a, b, 13));
#endif
  }
  
  // General functions
  //! Blend two vectors given choice. Takes from a if c is true, b if false.
  friend inline self_t blend(const self_t& a, const self_t& b, const bool_t& c) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(blend(a.data.v0_3, b.data.v0_3, c.data.v0_3),
                  blend(a.data.v4_7, b.data.v4_7, c.data.v4_7));
#else
    return _mm256_blendv_ps(b, a, _mm256_castsi256_ps(c));
#endif
  }
  //! Returns the sum of all elements
  friend inline scalar_t horizontal_add(const self_t& a) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return horizontal_add(a.data.v0_3) + horizontal_add(a.data.v4_7);
#else
    intrinsic_t tmp = _mm256_hadd_ps(a.data, a.data);
    tmp = _mm256_hadd_ps(tmp, tmp);
    return half_t(_mm256_extractf128_ps(tmp, 0) +
                  _mm256_extractf128_ps(tmp, 1))[0];
#endif
  }
  //! Find the maximum elements between two vectors
  friend inline self_t max(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(max(a.data.v0_3, b.data.v0_3),
                  max(a.data.v4_7, b.data.v4_7));
#else
    return _mm256_max_ps(a, b);
#endif
  }
  //! Find the maximum element in a vector
  friend inline scalar_t horizontal_max(const self_t& a) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return SVL_MAX(horizontal_max(a.data.v0_3),
               horizontal_max(a.data.v4_7));
#else
    return SVL_MAX(horizontal_max(half_t(_mm256_extractf128_ps(a.data, 0))),
               horizontal_max(half_t(_mm256_extractf128_ps(a.data, 1))));
#endif
  }
  //! Find the minimum elements between two vectors
  friend inline self_t min(const self_t& a, const self_t& b) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(min(a.data.v0_3, b.data.v0_3),
                  min(a.data.v4_7, b.data.v4_7));
#else
    return _mm256_min_ps(a, b);
#endif
  }
  //! Find the minimum element in a vector
  friend inline scalar_t horizontal_min(const self_t& a) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return SVL_MIN(horizontal_min(a.data.v0_3),
               horizontal_min(a.data.v4_7));
#else
    return SVL_MIN(horizontal_min(half_t(_mm256_extractf128_ps(a.data, 0))),
               horizontal_min(half_t(_mm256_extractf128_ps(a.data, 1))));
#endif
  }
  
  // Special math functions
  //! Finds the square root of all elements in a
  friend inline self_t sqrt(const self_t& a) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(sqrt(a.data.v0_3), sqrt(a.data.v4_7));
#else
    return _mm256_sqrt_ps(a);
#endif
  }
  
  //! Calculates the sine of all elements in x
  friend inline self_t sin(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(sin(x.data.v0_3), sin(x.data.v4_7));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(sin(x[i]), i);
    return r;
#endif
  }
  
  //! Calculates the cosine of all elements in x
  friend inline self_t cos(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(cos(x.data.v0_3), cos(x.data.v4_7));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(cos(x[i]), i);
    return r;
#endif
  }
  //! Calculates the tangent of all elements in x
  friend inline self_t tan(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(tan(x.data.v0_3), tan(x.data.v4_7));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(tan(x[i]), i);
    return r;
#endif
  }
  //! Calculates the arcsine of all elements in x
  friend inline self_t asin(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(asin(x.data.v0_3), asin(x.data.v4_7));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(asin(x[i]), i);
    return r;
#endif
  }
  
  //! Calculates the arccosine of all elements in x
  friend inline self_t acos(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(acos(x.data.v0_3), acos(x.data.v4_7));
#else
    // Constants
    const __m256 abs_mask = _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF));
    const __m256 sign_mask = _mm256_castsi256_ps(_mm256_set1_epi32(0x80000000));
    const __m256 halves = _mm256_set1_ps(0.5f);
    const __m256 zeroes = _mm256_setzero_ps();
    const __m256 pis = _mm256_set1_ps(3.14159265358979323846264338327950288419716939937510582097f);
    const __m256 half_pis = _mm256_set1_ps(3.14159265358979323846264338327950288419716939937510582097f * 0.5f);

    const __m256 C4 = _mm256_set1_ps(4.2163199048E-2f);
    const __m256 C3 = _mm256_set1_ps(2.4181311049E-2f);
    const __m256 C2 = _mm256_set1_ps(4.5470025998E-2f);
    const __m256 C1 = _mm256_set1_ps(7.4953002686E-2f);
    const __m256 C0 = _mm256_set1_ps(1.6666752422E-1f);
    // End constants
    
    __m256 abs_x = _mm256_and_ps(x, abs_mask);
    __m256 selector = _mm256_cmp_ps(abs_x, halves, 14);
    __m256 x1 = _mm256_fnmadd_ps(halves, abs_x, halves);
    __m256 x2 = _mm256_mul_ps(abs_x, abs_x);
    __m256 x3 = _mm256_blendv_ps(x2, x1, selector);
    __m256 sqrt_x1 = _mm256_sqrt_ps(x1);
    __m256 x4 = _mm256_blendv_ps(abs_x, sqrt_x1, selector);

    // Calculate c4*x^4 + c3*x^3 + c2*x^2 + c1*x + c0 where x = x3
    __m256 x3_2 = _mm256_mul_ps(x3, x3);
    __m256 x3_4 = _mm256_mul_ps(x3_2, x3_2);
    __m256 c1_x_x3_p_c0 = _mm256_fmadd_ps(C1, x3, C0);
    __m256 c3_x_x3_p_c2 = _mm256_fmadd_ps(C3, x3, C2);
    __m256 c4_x_x34_p_y = _mm256_fmadd_ps(C4, x3_4, c1_x_x3_p_c0);
    __m256 res = _mm256_fmadd_ps(c3_x_x3_p_c2, x3_2, c4_x_x34_p_y);
    res = _mm256_fmadd_ps(res, _mm256_mul_ps(x3, x4), x4);
    __m256 res1 = _mm256_add_ps(res, res);

    __m256 selector_2 = _mm256_cmp_ps(x, zeroes, 1);
    __m256 pi_minus_res1 = _mm256_sub_ps(pis, res1);
    res1 = _mm256_blendv_ps(res1, pi_minus_res1, selector_2);

    __m256 res2 = _mm256_and_ps(x, sign_mask);
    res = _mm256_xor_ps(res, res2);
    res2 = _mm256_sub_ps(half_pis, res);
    res = _mm256_blendv_ps(res2, res1, selector);
    return res;
#endif
  }
  //! Calculates the arctangent of all elements in x
  friend inline self_t atan(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(atan(x.data.v0_3), atan(x.data.v4_7));
#else
    self_t r;
    SVL_FOR_RANGE(step) r.assign(atan(x[i]), i);
    return r;
#endif
  }
  //! Calculates the arctangent of all elements in y/x determining the correct quadrant
  friend inline self_t atan2(const self_t& y, const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(atan2(y.data.v0_3, x.data.v0_3),
                  atan2(y.data.v4_7, x.data.v4_7));
#else
    // Constants
    const __m256 C3 = _mm256_set1_ps(8.05374449538E-2f);
    const __m256 C2 = _mm256_set1_ps(-1.38776856032E-1f);
    const __m256 C1 = _mm256_set1_ps(1.99777106478E-1f);
    const __m256 C0 = _mm256_set1_ps(-3.33329491539E-1f);
    const __m256 piover2 = _mm256_set1_ps(1.57079632679489661923f);
    const __m256 pi = _mm256_set1_ps(3.14159265358979323846f);
    const __m256 sqrt2_minus1 = _mm256_set1_ps(0.41421356237309504880f);
    const __m256 minus1 = _mm256_set1_ps(-1.f);
    const __m256 plus1 = _mm256_set1_ps(1.f);
    const __m256 piover4 = _mm256_set1_ps(0.785398163397448309616f);
    const __m256 sign_mask = _mm256_castsi256_ps(_mm256_set1_epi32(0x80000000));
    // End constants
    
    __m256 x1 = abs(x), y1 = abs(y);
    __m256 swapxy = _mm256_castps_si256(_mm256_cmp_ps(y1, x1, 14));
    __m256 x2 = _mm256_blendv_ps(x1, y1, swapxy);
    __m256 y2 = _mm256_blendv_ps(y1, x1, swapxy);
    __m256 t = _mm256_div_ps(y2, x2);
    
    __m256 notsmall = _mm256_castps_si256(_mm256_cmp_ps(t, sqrt2_minus1, 13));
    __m256 a = _mm256_add_ps(t, _mm256_and_ps(notsmall, minus1));
    __m256 b = _mm256_add_ps(plus1, _mm256_and_ps(notsmall, t));
    __m256 s = _mm256_and_ps(notsmall, piover4);
    __m256 z = _mm256_div_ps(a, b);
    
    __m256 zsq = _mm256_mul_ps(z, z);
    
    // Polynomial 3
    __m256 zsqsq = _mm256_mul_ps(zsq, zsq);
    __m256 fma1 = _mm256_fmadd_ps(C3, zsq, C2);
    __m256 fma2 = _mm256_fmadd_ps(C1, zsq, C0);
    __m256 res = _mm256_fmadd_ps(fma1, zsqsq, fma2);
    res = _mm256_fmadd_ps(res, _mm256_mul_ps(zsq, z), z);
    res = _mm256_add_ps(res, s);
    
    res = _mm256_blendv_ps(res, _mm256_sub_ps(piover2, res), swapxy);
    res = _mm256_blendv_ps(res, _mm256_sub_ps(pi, res), _mm256_castps_si256(_mm256_cmp_ps(x, _mm256_setzero_ps(), 1)));
    res = _mm256_blendv_ps(res, _mm256_setzero_ps(), _mm256_castps_si256(_mm256_cmp_ps(_mm256_or_ps(x, y), _mm256_setzero_ps(), 0)));
    
    res = _mm256_xor_ps(res, _mm256_and_ps(y, sign_mask));
    return res;
#endif
  }
  //! Remove the sign bit from all elements in x
  friend inline self_t abs(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(abs(x.data.v0_3), abs(x.data.v4_7));
#else
    __m256i mask = _mm256_set_epi32(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF,
                                    0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF);
    return _mm256_and_ps(_mm256_castsi256_ps(mask), x.data);
#endif
  }
  
  //! Floor of the values of x
  friend inline self_t floor(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(floor(x.data.v0_3), floor(x.data.v4_7));
#else
    return _mm256_floor_ps(x.data);
#endif
  }
  //! Ceil of the values of x
  friend inline self_t ceil(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(ceil(x.data.v0_3), ceil(x.data.v4_7));
#else
    return _mm256_ceil_ps(x.data);
#endif
  }
  
  //! Round the values of x
  friend inline self_t round(const self_t& x) {
#if SVL_SIMD_LEVEL < SVL_AVX2
    return self_t(round(x.data.v0_3), round(x.data.v4_7));
#else
    return _mm256_round_ps(x.data, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
#endif
  }
  
};


#ifdef DEBUG
static inline std::ostream& operator<<(std::ostream& os, const Vector8f& v) {
  os << "<";
  SVL_FOR_RANGE(v.step) os << v[i] << ((i < (v.step - 1)) ? ", " : "");
  os << ">";
  return os;
}
#endif
