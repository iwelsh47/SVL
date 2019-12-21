#ifndef __SVL_HEADER_INCLUDED__
#error Please include the SVL.h header only
#endif
#pragma once
#include <ostream>

#if SVL_INSTRUCTION_SET < __SVL_AVX512F__
#include <array>
#endif

namespace SVL {
/** \brief A vector of 16 booleans.
 *  \details To enable the use of full vector instructions, a minimum level of AVX512F vector instructions must be enabled at compile time. Certain methods and functions are improved when higher levels of vector instructions are enabled.
 */
class Vector16b {
public:
  //! The number of elements in the vector
  static constexpr uint64_t width = 16;
  //! The number of elements in half the vector
  static constexpr uint64_t half_width = 8;
  
  //! The type of the vector
  using self_t = Vector16b;
  //! The type of half the vector.
  using half_t = Vector8b;
#if __USE_AVX512__
  using intrinsic_t = __mmask16;
#else
  using intrinsic_t = std::array<half_t, 2>;
#endif
  intrinsic_t data;
  
  //! Compile time constant constructor
  template<bool B0, bool B1 = B0, bool B2 = B1, bool B3 = B2,
           bool B4 = B3, bool B5 = B4, bool B6 = B5, bool B7 = B6,
           bool B8 = B7, bool B9 = B8, bool Ba = B9, bool Bb = Ba,
           bool Bc = Bb, bool Bd = Bc, bool Be = Bd, bool Bf = Be>
  static self_t const_();
  
  //! Compile time constant vector with all true elements
  static self_t true_();
  //! Compile time constant vector with all false elements
  static self_t false_();
  
  //! Default constructor
  Vector16b();
  //! Destructor
  ~Vector16b();
  //! Copy constructor
  Vector16b(self_t & v);
  Vector16b(const self_t &v);
  Vector16b(intrinsic_t &v);
  Vector16b(const intrinsic_t &v);
  Vector16b(half_t& v0, half_t& v1);
  Vector16b(const half_t& v0, const half_t& v1);
  //! Move constructor
  Vector16b(self_t &&v);
  Vector16b(intrinsic_t &&v);
  Vector16b(half_t&& v0, half_t&& v1);
  
  //! Broadcast a value to all elements
  Vector16b(bool v);
  //! Construct from the given values
  Vector16b(bool v0, bool v1, bool v2, bool v3, bool v4, bool v5, bool v6, bool v7,
            bool v8, bool v9, bool va, bool vb, bool vc, bool vd, bool ve, bool vf);
  //! Construct from an array
  Vector16b(bool *a);
  Vector16b(const bool *a);
  //! Prevent construction from anything except explicitly defined
  template<typename... Args> Vector16b(Args &&...) = delete;
  
  //! Copy assignment
  self_t &operator=(self_t &v);
  self_t &operator=(const self_t &v);
  self_t &operator=(intrinsic_t &v);
  self_t &operator=(const intrinsic_t &v);
  //! Move assignment
  self_t &operator=(self_t &&v);
  self_t &operator=(intrinsic_t &&v);
  //! Assign bool
  self_t &operator=(bool v);
  //! Prevent assignment from anything except explicitly defined
  template<typename... Args> Vector4b &operator=(Args &&...) = delete;
  
  //! Covert to intrinsic type
  operator intrinsic_t() const;
  //! Split in half
  half_t get_low() const;
  half_t get_high() const;
  
  //! Broadcast a single value to all elements
  self_t &broadcast(bool v);
  //! Load up to 8 given values, falsing out the rest
  self_t &load(bool v0 = false, bool v1 = false, bool v2 = false, bool v3 = false,
               bool v4 = false, bool v5 = false, bool v6 = false, bool v7 = false,
               bool v8 = false, bool v9 = false, bool va = false, bool vb = false,
               bool vc = false, bool vd = false, bool ve = false, bool vf = false);
  //! Load 8 values from an array
  self_t &load(const bool *a);
  //! Load n values from an array, falsing out the rest
  self_t &load_partial(const bool *a, uint64_t n);
  
  //! Store values in an array
  void store(bool *a) const;
  //! Store n values in an array
  void store_partial(bool *a, uint64_t n) const;
  
  //! Access a single value
  bool access(uint64_t idx) const;
  
  //! Assign a single value
  self_t &assign(bool v, uint64_t idx);
  
  // Logical AND operators
  self_t and_(const self_t &b) const;
  self_t operator&(const self_t &b) const;
  self_t operator&&(const self_t &b) const;
  self_t &operator&=(const self_t &b);
  
  // Logical OR operators
  self_t or_(const self_t &b) const;
  self_t operator|(const self_t &b) const;
  self_t operator||(const self_t &b) const;
  self_t &operator|=(const self_t &b);
  
  // Logical XOR operators
  self_t xor_(const self_t &b) const;
  self_t operator^(const self_t &b) const;
  self_t &operator^=(const self_t &b);
  
  // Logical NOT operator (Bitwise)
  self_t not_() const;
  self_t operator~() const;
  self_t operator!() const;
  
  // Logical ANDNOT operator
  self_t and_not_(const self_t &b) const;
  
  // Horizontal functions
  bool all() const;
  bool any() const;
  bool none() const;
};

Vector16b blend(const Vector16b& a, const Vector16b& b, const Vector16b& mask);
}  // namespace SVL
