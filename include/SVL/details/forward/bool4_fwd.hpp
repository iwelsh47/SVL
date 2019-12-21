#ifndef __SVL_HEADER_INCLUDED__
  #error Please include the SVL.h header only
#endif
#pragma once
#include <ostream>

#if SVL_INSTRUCTION_SET < __SVL_SSE2__
#include <array>
#endif

namespace SVL {
  /** \brief A vector of 4 booleans.
   *  \details To enable the use of vector instructions, a minimum level of SSE2 vector instructions must be enabled at compile time. Certain methods and functions are improved when higher levels of vector instructions are enabled.
   */
  class Vector4b {
  public:
    //! The number of elements in the vector
    static constexpr uint64_t width = 4;
    //! The number of elements in half the vector
    static constexpr uint64_t half_width = 2;
    
    //! The type of the vector
    using self_t = Vector4b;
    //! The type of half the vector. As no Vector2b available, this is null
    using half_t = std::nullptr_t;
#if __USE_AVX512__
    using intrinsic_t = __mmask8;
#elif __USE_SSE2__
    using intrinsic_t = __m128i;
#else
    using intrinsic_t = std::array<bool, width>;
#endif
    intrinsic_t data;

    //! Compile time constant constructor
    template<bool B0, bool B1 = B0, bool B2 = B1, bool B3 = B2>
    static self_t const_();
    
    //! Compile time constant vector with all true elements
    static self_t true_();
    //! Compile time constant vector with all false elements
    static self_t false_();

    //! Default constructor
    Vector4b() = default;
    //! Destructor
    ~Vector4b() = default;
    //! Copy constructor
    Vector4b(self_t & v) = default;
    Vector4b(const self_t &v) = default;
    Vector4b(intrinsic_t &v);
    Vector4b(const intrinsic_t &v);
    //! Move constructor
    Vector4b(self_t &&v) = default;
    Vector4b(intrinsic_t &&v);

    //! Broadcast a value to all elements
    Vector4b(bool v);
    //! Construct from the given values
    Vector4b(bool v0, bool v1, bool v2, bool v3);
    //! Construct from an array
    Vector4b(bool *a);
    Vector4b(const bool *a);
    //! Prevent construction from anything except explicitly defined
    template<typename... Args> Vector4b(Args &&...) = delete;
    
    //! Copy assignment
    self_t &operator=(self_t &v) = default;
    self_t &operator=(const self_t &v) = default;
    self_t &operator=(intrinsic_t &v);
    self_t &operator=(const intrinsic_t &v);
    //! Move assignment
    self_t &operator=(self_t &&v) = default;
    self_t &operator=(intrinsic_t &&v);
    //! Assign bool
    self_t &operator=(bool v);
    //! Prevent assignment from anything except explicitly defined
    template<typename... Args> Vector4b &operator=(Args &&...) = delete;
    
    //! Covert to intrinsic type
    operator intrinsic_t() const;

    //! Broadcast a single value to all elements
    self_t &broadcast(bool v);
    //! Load up to 4 given values, falsing out the rest
    self_t &load(bool v0 = false, bool v1 = false, bool v2 = false, bool v3 = false);
    //! Load 4 values from an array
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

Vector4b blend(const Vector4b& a, const Vector4b& b, const Vector4b& mask);
}  // namespace SVL
