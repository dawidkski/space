#pragma once
#include "tensor_forward.hpp"

namespace ts {

auto multiply(Matrix const &, Vector const &) -> Vector;

auto multiply(Matrix const & A, Matrix const & B, bool A_T=false, bool B_T=false) -> Matrix ;

auto multiply(Tensor<float, 3> const & A, Matrix const & B) -> Tensor<float, 3>;

}