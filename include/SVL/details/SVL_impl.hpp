#ifndef __SVL_HEADER_INCLUDED__
#error Please include the SVL.h header only
#endif

#define __SVL_HEADER_IMPLEMENTATION_INCLUDED__ 1

#include "implmentation/bool4_impl.hpp"
#include "implmentation/bool8_impl.hpp"

namespace SVL {
template <typename VectorType, uint64_t VectorSize = VectorType::width>
std::ostream& operator<<(std::ostream& os, const VectorType& vec) {
  os << '<';
  SVL_FOR_RANGE (i, VectorSize) {
    os << (vec.access(i) ? "true" : "false");
    os << ((i + 1 == VectorSize) ? "" : ",");
  }
  os << '>';
  return os;
}
}
