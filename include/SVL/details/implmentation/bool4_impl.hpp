#ifndef __SVL_HEADER_IMPLEMENTATION_INCLUDED__
#error Please include the SVL.h header only
#endif

#include "../forward/bool4_fwd.hpp"
#include <cassert>
#include <limits>

namespace SVL {
using self_4b = Vector4b::self_t;
//using intrinsic_t = Vector4b::intrinsic_t;

// Compile time constants
template<bool B0, bool B1, bool B2, bool B3>
#if __USE_AVX512__
inline self_4b Vector4b::const_() {
  return intrinsic_t((uint8_t)B0 | (uint8_t)B1 << 1 | (uint8_t)B2 << 2 | (uint8_t)B3 << 3);
}
#elif __USE_SSE2__
inline self_4b Vector4b::const_() {
  return _mm_setr_epi32(B0 ? -1 : 0, B1 ? -1 : 0, B2 ? -1 : 0, B3 ? -1 : 0);
}
#else
inline self_4b Vector4b::const_() { return {B0, B1, B2, B3}; }
#endif

inline self_4b Vector4b::true_() { return const_<true>(); }
inline self_4b Vector4b::false_() { return const_<false>(); }

// Constructors
inline Vector4b::Vector4b(intrinsic_t& v) : data(v) {}
inline Vector4b::Vector4b(const intrinsic_t& v) : data(v) {}
inline Vector4b::Vector4b(intrinsic_t&& v) : data(std::move(v)) {}
inline Vector4b::Vector4b(const bool *a) : Vector4b(a[0], a[1], a[2], a[3]) {}
inline Vector4b::Vector4b(bool *a) : Vector4b(a[0], a[1], a[2], a[3]) {}
#if __USE_AVX512__
inline Vector4b::Vector4b(bool v) : data(v ? 0x0F : 0) {}
inline Vector4b::Vector4b(bool v0, bool v1, bool v2, bool v3)
: data((uint8_t)v0 | (uint8_t)v1 << 1 | (uint8_t)v2 << 2 | (uint8_t)v3 << 3) {}
#elif __USE_SSE2__
inline Vector4b::Vector4b(bool v) : data(_mm_set1_epi32(v ? -1 : 0)) {}
inline Vector4b::Vector4b(bool v0, bool v1, bool v2, bool v3)
: data(_mm_setr_epi32(v0 ? -1 : 0, v1 ? -1 : 0, v2 ? -1 : 0, v3 ? -1 : 0)) {}
#else
inline Vector4b::Vector4b(bool v) : data({v, v, v, v}) {}
inline Vector4b::Vector4b(bool v0, bool v1, bool v2, bool v3) : data({v0, v1, v2, v3}) {}
#endif

// Assignment
inline self_4b& Vector4b::operator=(intrinsic_t &v) { data = v; return *this; }
inline self_4b& Vector4b::operator=(const intrinsic_t &v) { data = v; return *this; }
inline self_4b& Vector4b::operator=(intrinsic_t &&v) { data = std::move(v); return *this; }
inline self_4b& Vector4b::operator=(bool v) { data = v ? true_().data : false_().data; return *this; }

// Conversion
inline Vector4b::operator intrinsic_t() const { return data; }

// Loading and storing
#if __USE_AVX512__
inline self_4b& Vector4b::broadcast(bool v) { data = v ? 0x0F : 0; return *this; }
inline self_4b& Vector4b::load(bool v0, bool v1, bool v2, bool v3) {
  data = (uint8_t)v0 | (uint8_t)v1 << 1 | (uint8_t)v2 << 2 | (uint8_t)v3 << 3;
  return *this;
}
inline self_4b& Vector4b::load(const bool *a) {
  data = (uint8_t)a[0] | (uint8_t)a[1] << 1 | (uint8_t)a[2] << 2 | (uint8_t)a[3] << 3;
  return *this;
}
inline void Vector4b::store_partial(bool *v, uint64_t n) const {
  assert(n <= width);
  SVL_FOR_RANGE(i, n) v[i] = bool(data & (1 << i));
}
#elif __USE_SSE2__
inline self_4b& Vector4b::broadcast(bool v) { data = _mm_set1_epi32(v ? -1 : 0); return *this; }
inline self_4b& Vector4b::load(bool v0, bool v1, bool v2, bool v3) {
  data = _mm_setr_epi32(v0 ? -1 : 0, v1 ? -1 : 0, v2 ? -1 : 0, v3 ? -1 : 0);
  return *this;
}
inline self_4b& Vector4b::load(const bool *a) {
  data = _mm_setr_epi32(a[0] ? -1 : 0, a[1] ? -1 : 0, a[2] ? -1 : 0, a[3] ? -1 : 0);
  return *this;
}
inline void Vector4b::store_partial(bool *a, uint64_t n) const {
  assert(n <= width);
  int32_t vec[width];
  _mm_storeu_si128((__m128i*)vec, data);
  SVL_FOR_RANGE(i, n) { a[i] = bool(vec[i]); }
}
#else
inline self_4b& Vector4b::broadcast(bool v) { data = {v, v, v, v}; return *this; }
inline self_4b& Vector4b::load(bool v0, bool v1, bool v2, bool v3) { data = {v0, v1, v2, v3}; return *this; }
inline self_4b& Vector4b::load(const bool *a) { data = {a[0], a[1], a[2], a[3]}; return *this; }
inline void Vector4b::store_partial(bool *a, uint64_t n) const {
  assert(n <= width);
  memcpy(a, &data, sizeof(bool) * n);
}
#endif
inline void Vector4b::store(bool *a) const { store_partial(a, width); }
inline self_4b& Vector4b::load_partial(const bool *a, uint64_t n) {
  assert(n <= width);
  switch (n) {
    case 0: load(); break;
    case 1: load(a[0]); break;
    case 2: load(a[0], a[1]); break;
    case 3: load(a[0], a[1], a[2]); break;
    case 4: load(a); break;
  }
  return *this;
}

// Access and assignment
#if __USE_AVX512__
inline bool Vector4b::access(uint64_t idx) const {
  assert(idx < width);
  return bool(data & (1 << idx));
}
inline self_4b& Vector4b::assign(bool v, uint64_t idx) {
  assert(idx < width);
  data = ((uint8_t)data & ~(1 << idx)) | ((uint8_t)v << idx);
  return *this;
}
#elif __USE_SSE41__
inline bool Vector4b::access(uint64_t idx) const {
  assert(idx < width);
  switch (idx) {
    case 0: return bool(_mm_extract_epi32(data, 0));
    case 1: return bool(_mm_extract_epi32(data, 1));
    case 2: return bool(_mm_extract_epi32(data, 2));
    default: return bool(_mm_extract_epi32(data, 3));
  }
}
inline self_4b& Vector4b::assign(bool v, uint64_t idx) {
  assert(idx < width);
  switch (idx) {
    case 0: data = _mm_insert_epi32(data, v ? -1 : 0, 0); break;
    case 1: data = _mm_insert_epi32(data, v ? -1 : 0, 1); break;
    case 2: data = _mm_insert_epi32(data, v ? -1 : 0, 2); break;
    default: data = _mm_insert_epi32(data, v ? -1 : 0, 3); break;
  }
  return *this;
}
#elif __USE_SSE2__
inline bool Vector4b::access(uint64_t idx) const {
  assert(idx < width);
  int32_t vec[width];
  _mm_storeu_si128((__m128i*)vec, data);
  return bool(vec[idx]);
}
inline self_4b& Vector4b::assign(bool v, uint64_t idx) {
  assert(idx < width);
  int32_t vec[width];
  _mm_storeu_si128((__m128i*)vec, data);
  vec[idx] = v ? -1 : 0;
  data = _mm_loadu_si128((const __m128i *)vec);
  return *this;
}
#else
inline bool Vector4b::access(uint64_t idx) const { assert(idx < width); return data[idx]; }
inline self_4b& Vector4b::assign(bool v, uint64_t idx) {
  assert(idx < width); data[idx] = v ? -1 : 0; return *this;
}
#endif

// Logical operators
#if __USE_AVX512__
inline self_4b Vector4b::and_(const self_t &b) const { return _kand_mask8(data, b); }
inline self_4b Vector4b::or_(const self_t &b) const { return _kor_mask8(data, b); }
inline self_4b Vector4b::xor_(const self_t &b) const { return _kxor_mask8(data, b); }
inline self_4b Vector4b::not_() const { return _knot_mask8(data); }
inline self_4b Vector4b::and_not_(const self_t &b) const { return _kandn_mask8(b, data); }
#elif __USE_SSE2__
inline self_4b Vector4b::and_(const self_t& b) const { return _mm_and_si128(data, b); }
inline self_4b Vector4b::or_(const self_t& b) const { return _mm_or_si128(data, b); }
inline self_4b Vector4b::xor_(const self_t& b) const { return _mm_xor_si128(data, b); }
inline self_4b Vector4b::not_() const { return _mm_andnot_si128(data, true_()); }
inline self_4b Vector4b::and_not_(const self_t& b) const { return _mm_andnot_si128(b, data); }
#else
inline self_4b Vector4b::and_(const self_t& b) const {
  intrinsic_t r; SVL_FOR_RANGE(i, width) { r[i] = data[i] & b.data[i]; } return r;
}
inline self_4b Vector4b::or_(const self_t& b) const {
  intrinsic_t r; SVL_FOR_RANGE(i, width) { r[i] = data[i] | b.data[i]; } return r;
}
inline self_4b Vector4b::xor_(const self_t& b) const {
  intrinsic_t r; SVL_FOR_RANGE(i, width) { r[i] = data[i] ^ b.data[i]; } return r;
}
inline self_4b Vector4b::not_() const {
  intrinsic_t r; SVL_FOR_RANGE(i, width) { r[i] = !data[i]; } return r;
}
inline self_4b Vector4b::and_not_(const self_t& b) const {
  intrinsic_t r; SVL_FOR_RANGE(i, width) { r[i] = data[i] & !b.data[i]; } return r;
}
#endif
inline self_4b Vector4b::operator&(const self_t& b) const { return and_(b); }
inline self_4b Vector4b::operator&&(const self_t& b) const { return and_(b); }
inline self_4b& Vector4b::operator&=(const self_t& b) { data = and_(b); return *this; }
inline self_4b Vector4b::operator|(const self_t& b) const { return or_(b); }
inline self_4b Vector4b::operator||(const self_t& b) const { return or_(b); }
inline self_4b& Vector4b::operator|=(const self_t& b) { data = or_(b); return *this; }
inline self_4b Vector4b::operator^(const self_t& b) const { return xor_(b); }
inline self_4b& Vector4b::operator^=(const self_t& b) { data = xor_(b); return *this; }
inline self_4b Vector4b::operator~() const { return not_(); }
inline self_4b Vector4b::operator!() const { return not_(); }

// Horizontal logicals
#if __USE_AVX512__
inline bool Vector4b::all() const { return _ktestc_mask8_u8(false_(), data); }
inline bool Vector4b::none() const { return _ktestz_mask8_u8(data, true_()); }
inline self_4b blend(const self_4b& a, const self_4b& b, const self_4b& mask) {
  return (mask & a) | (~mask & b);
}
#elif __USE_SSE41__
inline bool Vector4b::all() const { return _mm_testc_si128(data, true_()); }
inline bool Vector4b::none() const { return _mm_testz_si128(data, true_()); }
inline self_4b blend(const self_4b& a, const self_4b& b, const self_4b& mask) {
  return _mm_blendv_epi8(b, a, mask);
}
#elif __USE_SSE2__
inline bool Vector4b::all() const { return _mm_movemask_epi8(data) == std::numeric_limits<uint16_t>::max(); }
inline bool Vector4b::none() const { return _mm_movemask_epi8(data) == 0; }
inline self_4b blend(const self_4b& a, const self_4b& b, const self_4b& mask) { return (mask & a) | (~mask & b); }
#else
inline bool Vector4b::all() const { for (bool v : data) { if (!v) return false; } return true; }
inline bool Vector4b::none() const { for (bool v : data) { if (v) return false; } return true; }
inline self_4b blend(const self_4b& a, const self_4b& b, const self_4b& mask) { return (mask & a) | (~mask & b); }
#endif
inline bool Vector4b::any() const { return !none(); }


}
