#ifndef __SVL_HEADER_IMPLEMENTATION_INCLUDED__
#error Please include the SVL.h header only
#endif

#include "../forward/bool8_fwd.hpp"
#include <cassert>
#include <limits>

namespace SVL {
using self_16b = Vector16b::self_t;
using half_16b = Vector16b::half_t;

// Compile time constants
template<bool B0, bool B1, bool B2, bool B3, bool B4, bool B5, bool B6, bool B7>
#if __USE_AVX512__
inline self_16b Vector16b::const_() {
  return intrinsic_t((uint8_t)B0 | (uint8_t)B1 << 1 | (uint8_t)B2 << 2 | (uint8_t)B3 << 3);
}
#elif __USE_AVX2__
inline self_16b Vector16b::const_() {
  return _mm256_setr_epi32(B0 ? -1 : 0, B1 ? -1 : 0, B2 ? -1 : 0, B3 ? -1 : 0,
                           B4 ? -1 : 0, B5 ? -1 : 0, B6 ? -1 : 0, B7 ? -1 : 0);
}
#else
inline self_16b Vector16b::const_() {
  return {half_t::const_<B0, B1, B2, B3>(), half_t::const_<B4, B5, B6, B7>()};
}
#endif

inline self_16b Vector16b::true_() { return const_<true>(); }
inline self_16b Vector16b::false_() { return const_<false>(); }

// Constructors
inline Vector16b::Vector16b(intrinsic_t& v) : data(v) {}
inline Vector16b::Vector16b(const intrinsic_t& v) : data(v) {}
inline Vector16b::Vector16b(intrinsic_t&& v) : data(std::move(v)) {}
inline Vector16b::Vector16b(const bool *a) : Vector16b(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]) {}
inline Vector16b::Vector16b(bool *a) : Vector16b(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]) {}
#if __USE_AVX512__
inline Vector16b::Vector16b(half_t& v0, half_t& v1) : data((v1 << half_width) | v0) {}
inline Vector16b::Vector16b(const half_t& v0, const half_t& v1) : data((v1 << half_width) | v0) {}
inline Vector16b::Vector16b(half_t&& v0, half_t&& v1) : data((v1 << half_width) | v0) {}
inline Vector16b::Vector16b(bool v) : data(v ? 0x0F : 0) {}
inline Vector16b::Vector16b(bool v0, bool v1, bool v2, bool v3, bool v4, bool v5, bool v6, bool v7)
: data((uint8_t)v0 | (uint8_t)v1 << 1 | (uint8_t)v2 << 2 | (uint8_t)v3 << 3) {}
#elif __USE_AVX2__
inline Vector16b::Vector16b(half_t& v0, half_t& v1) : data(_mm256_loadu2_m128i(&v1.data, &v0.data)) {}
inline Vector16b::Vector16b(const half_t& v0, const half_t& v1)
: data(_mm256_loadu2_m128i(&v1.data, &v0.data)) {}
inline Vector16b::Vector16b(half_t&& v0, half_t&& v1) {
  half_t::intrinsic_t low = std::move(v0.data);
  half_t::intrinsic_t high = std::move(v1.data);
  data = _mm256_loadu2_m128i(&high, &low);
}
inline Vector16b::Vector16b(bool v) : data(_mm256_set1_epi32(v ? -1 : 0)) {}
inline Vector16b::Vector16b(bool v0, bool v1, bool v2, bool v3, bool v4, bool v5, bool v6, bool v7)
: data(_mm256_setr_epi32(v0 ? -1 : 0, v1 ? -1 : 0, v2 ? -1 : 0, v3 ? -1 : 0,
                         v4 ? -1 : 0, v5 ? -1 : 0, v6 ? -1 : 0, v7 ? -1 : 0)) {}
#else
inline Vector16b::Vector16b(half_t& v0, half_t& v1) : data({v0,v1}) {}
inline Vector16b::Vector16b(const half_t& v0, const half_t& v1) : data({v0,v1}) {}
inline Vector16b::Vector16b(half_t&& v0, half_t&& v1) : data({std::move(v0),std::move(v1)}) {}
inline Vector16b::Vector16b(bool v) : data({half_t(v), half_t(v)}) {}
inline Vector16b::Vector16b(bool v0, bool v1, bool v2, bool v3, bool v4, bool v5, bool v6, bool v7)
: data({half_t(v0, v1, v2, v3), half_t(v4, v5, v6, v7)}) {}
#endif

// Assignment
inline self_16b& Vector16b::operator=(intrinsic_t &v) { data = v; return *this; }
inline self_16b& Vector16b::operator=(const intrinsic_t &v) { data = v; return *this; }
inline self_16b& Vector16b::operator=(intrinsic_t &&v) { data = std::move(v); return *this; }
inline self_16b& Vector16b::operator=(bool v) { data = v ? true_().data : false_().data; return *this; }

// Conversion
inline Vector16b::operator intrinsic_t() const { return data; }

// Loading and storing
#if __USE_AVX512__
inline self_16b& Vector16b::broadcast(bool v) { data = v ? 0x0F : 0; return *this; }
inline self_16b& Vector16b::load(bool v0, bool v1, bool v2, bool v3, bool v4, bool v5, bool v6, bool v7) {
  data = (uint8_t)v0 | (uint8_t)v1 << 1 | (uint8_t)v2 << 2 | (uint8_t)v3 << 3;
  return *this;
}
inline self_16b& Vector16b::load(const bool *a) {
  data = (uint8_t)a[0] | (uint8_t)a[1] << 1 | (uint8_t)a[2] << 2 | (uint8_t)a[3] << 3;
  return *this;
}
inline void Vector16b::store_partial(bool *v, uint64_t n) const {
  assert(n <= width);
  SVL_FOR_RANGE(i, n) v[i] = bool(data & (1 << i));
}
inline self_16b& Vector16b::load_partial(const bool *a, uint64_t n) {
  assert(n <= width);
  data = 0;
  SVL_FOR_RANGE(i, n) data |= (1 << i) & a[i];
  return *this;
}
#elif __USE_AVX2__
inline self_16b& Vector16b::broadcast(bool v) { data = _mm256_set1_epi32(v ? -1 : 0); return *this; }
inline self_16b& Vector16b::load(bool v0, bool v1, bool v2, bool v3, bool v4, bool v5, bool v6, bool v7) {
  data = _mm256_setr_epi32(v0 ? -1 : 0, v1 ? -1 : 0, v2 ? -1 : 0, v3 ? -1 : 0,
                           v4 ? -1 : 0, v5 ? -1 : 0, v6 ? -1 : 0, v7 ? -1 : 0);
  return *this;
}
inline self_16b& Vector16b::load(const bool *a) {
  data = _mm256_setr_epi32(a[0] ? -1 : 0, a[1] ? -1 : 0, a[2] ? -1 : 0, a[3] ? -1 : 0,
                           a[4] ? -1 : 0, a[5] ? -1 : 0, a[6] ? -1 : 0, a[7] ? -1 : 0);
  return *this;
}
inline void Vector16b::store_partial(bool *a, uint64_t n) const {
  assert(n <= width);
  int32_t vec[width];
  _mm256_storeu_si256((__m256i*)vec, data);
  SVL_FOR_RANGE(i, n) { a[i] = bool(vec[i]); }
}
inline self_16b& Vector16b::load_partial(const bool *a, uint64_t n) {
  //! \todo look at if theres a better way to do this
  assert(n <= width);
  half_t low, high = half_t::false_();
  if (n > half_width) {
    low.load(a);
    high.load_partial(a + half_width, n - half_width);
  } else {
    low.load_partial(a, n);
  }
  data = _mm256_loadu2_m128i(&high.data, &low.data);
  return *this;
}
#else
inline self_16b& Vector16b::broadcast(bool v) {
  data[0].broadcast(v);
  data[1].broadcast(v);
  return *this;
}
inline self_16b& Vector16b::load(bool v0, bool v1, bool v2, bool v3, bool v4, bool v5, bool v6, bool v7) {
  data[0].load(v0, v1, v2, v3);
  data[1].load(v4, v5, v6, v7);
  return *this;
}
inline self_16b& Vector16b::load(const bool *a) {
  data[0].load(a);
  data[1].load(a + half_width);
  return *this;
}
inline void Vector16b::store_partial(bool *a, uint64_t n) const {
  assert(n <= width);
  if (n > half_width) {
    data[0].store_partial(a, half_width);
    data[1].store_partial(a + half_width, n - half_width);
  } else {
    data[0].store_partial(a, n);
  }
}
inline self_16b& Vector16b::load_partial(const bool *a, uint64_t n) {
  assert(n <= width);
  if (n > half_width) {
    data[0].load(a);
    data[1].load_partial(a + half_width, n - half_width);
  } else {
    data[0].load_partial(a, n);
    data[1] = false;
  }
  return *this;
}
#endif
inline void Vector16b::store(bool *a) const { store_partial(a, width); }


// Access and assignment
#if __USE_AVX512__
inline bool Vector16b::access(uint64_t idx) const {
  assert(idx < width);
  return bool(data & (1 << idx));
}
inline self_16b& Vector16b::assign(bool v, uint64_t idx) {
  assert(idx < width);
  data = ((uint8_t)data & ~(1 << idx)) | ((uint8_t)v << idx);
  return *this;
}
inline half_16b Vector16b::get_low() const {
  
}
inline half_16b Vector16b::get_high() const {
  
}
#elif __USE_AVX2__
inline bool Vector16b::access(uint64_t idx) const {
  assert(idx < width);
  switch (idx) {
    case 0: return bool(_mm256_extract_epi32(data, 0));
    case 1: return bool(_mm256_extract_epi32(data, 1));
    case 2: return bool(_mm256_extract_epi32(data, 2));
    case 3: return bool(_mm256_extract_epi32(data, 3));
    case 4: return bool(_mm256_extract_epi32(data, 4));
    case 5: return bool(_mm256_extract_epi32(data, 5));
    case 6: return bool(_mm256_extract_epi32(data, 6));
    default: return bool(_mm256_extract_epi32(data, 7));
  }
}
inline self_16b& Vector16b::assign(bool v, uint64_t idx) {
  assert(idx < width);
  switch (idx) {
    case 0: data = _mm256_insert_epi32(data, v ? -1 : 0, 0); break;
    case 1: data = _mm256_insert_epi32(data, v ? -1 : 0, 1); break;
    case 2: data = _mm256_insert_epi32(data, v ? -1 : 0, 2); break;
    case 3: data = _mm256_insert_epi32(data, v ? -1 : 0, 3); break;
    case 4: data = _mm256_insert_epi32(data, v ? -1 : 0, 4); break;
    case 5: data = _mm256_insert_epi32(data, v ? -1 : 0, 5); break;
    case 6: data = _mm256_insert_epi32(data, v ? -1 : 0, 6); break;
    default: data = _mm256_insert_epi32(data, v ? -1 : 0, 7); break;
  }
  return *this;
}
inline half_16b Vector16b::get_low() const { return _mm256_extracti128_si256(data, 0); }
inline half_16b Vector16b::get_high() const { return _mm256_extracti128_si256(data, 1); }
#else
inline bool Vector16b::access(uint64_t idx) const {
  assert(idx < width);
  if (idx < half_width) return data[0].access(idx);
  else return data[1].access(idx - half_width);
}
inline self_16b& Vector16b::assign(bool v, uint64_t idx) {
  assert(idx < width);
  if (idx < half_width) data[0].assign(v, idx);
  else data[1].assign(v, idx - half_width);
  return *this;
}
inline half_16b Vector16b::get_low() const { return data[0]; }
inline half_16b Vector16b::get_high() const { return data[1]; }
#endif

// Logical operators
#if __USE_AVX512__
inline self_16b Vector16b::and_(const self_t &b) const { return _kand_mask8(data, b); }
inline self_16b Vector16b::or_(const self_t &b) const { return _kor_mask8(data, b); }
inline self_16b Vector16b::xor_(const self_t &b) const { return _kxor_mask8(data, b); }
inline self_16b Vector16b::not_() const { return _knot_mask8(data); }
inline self_16b Vector16b::and_not_(const self_t &b) const { return _kandn_mask8(b, data); }
#elif __USE_AVX2__
inline self_16b Vector16b::and_(const self_t& b) const { return _mm256_and_si256(data, b); }
inline self_16b Vector16b::or_(const self_t& b) const { return _mm256_or_si256(data, b); }
inline self_16b Vector16b::xor_(const self_t& b) const { return _mm256_xor_si256(data, b); }
inline self_16b Vector16b::not_() const { return _mm256_andnot_si256(data, true_()); }
inline self_16b Vector16b::and_not_(const self_t& b) const { return _mm256_andnot_si256(b, data); }
#else
inline self_16b Vector16b::and_(const self_t& b) const {
  intrinsic_t r;
  r[0] = data[0].and_(b.data[0]);
  r[1] = data[1].and_(b.data[1]);
  return r;
}
inline self_16b Vector16b::or_(const self_t& b) const {
  intrinsic_t r;
  r[0] = data[0].or_(b.data[0]);
  r[1] = data[1].or_(b.data[1]);
  return r;
}
inline self_16b Vector16b::xor_(const self_t& b) const {
  intrinsic_t r;
  r[0] = data[0].xor_(b.data[0]);
  r[1] = data[1].xor_(b.data[1]);
  return r;
}
inline self_16b Vector16b::not_() const {
  intrinsic_t r;
  r[0] = data[0].not_();
  r[1] = data[1].not_();
  return r;
}
inline self_16b Vector16b::and_not_(const self_t& b) const {
  intrinsic_t r;
  r[0] = data[0].and_not_(b.data[0]);
  r[1] = data[1].and_not_(b.data[1]);
  return r;
}
#endif
inline self_16b Vector16b::operator&(const self_t& b) const { return and_(b); }
inline self_16b Vector16b::operator&&(const self_t& b) const { return and_(b); }
inline self_16b& Vector16b::operator&=(const self_t& b) { data = and_(b); return *this; }
inline self_16b Vector16b::operator|(const self_t& b) const { return or_(b); }
inline self_16b Vector16b::operator||(const self_t& b) const { return or_(b); }
inline self_16b& Vector16b::operator|=(const self_t& b) { data = or_(b); return *this; }
inline self_16b Vector16b::operator^(const self_t& b) const { return xor_(b); }
inline self_16b& Vector16b::operator^=(const self_t& b) { data = xor_(b); return *this; }
inline self_16b Vector16b::operator~() const { return not_(); }
inline self_16b Vector16b::operator!() const { return not_(); }

// Horizontal logicals
#if __USE_AVX512__
inline bool Vector16b::all() const { return _ktestc_mask8_u8(false_(), data); }
inline bool Vector16b::none() const { return _ktestz_mask8_u8(data, true_()); }
inline self_16b blend(const self_16b& a, const self_16b& b, const self_16b& mask) {
  return (mask & a) | (~mask & b);
}
#elif __USE_AVX2__
inline bool Vector16b::all() const {
  return _mm256_movemask_epi8(data) == -1; }
inline bool Vector16b::none() const { return _mm256_movemask_epi8(data) == 0; }
inline self_16b blend(const self_16b& a, const self_16b& b, const self_16b& mask) {
  return _mm256_blendv_epi8(b, a, mask);
}
#else
inline bool Vector16b::all() const { return data[0].all() && data[1].all(); }
inline bool Vector16b::none() const { return data[0].none() && data[1].none(); }
inline self_16b blend(const self_16b& a, const self_16b& b, const self_16b& mask) {
  return (mask & a) | (~mask & b);
}
#endif
inline bool Vector16b::any() const { return !none(); }


}
