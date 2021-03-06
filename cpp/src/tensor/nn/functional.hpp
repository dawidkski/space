#pragma once

#include <tensor/tensor.hpp>

namespace ts {

auto conv_2d(Tensor<float, 4> const &image, Tensor<float, 2> const &kernel, int kernel_size,
             int stride) -> Tensor<float, 4>;

auto conv_2d(Tensor<float, 3> const &image, Tensor<float, 2> const &kernel, int kernel_size,
             int stride) -> Tensor<float, 3>;

auto conv_2d(MatrixF const &matrix, MatrixF const &kernel, int stride) -> MatrixF;

auto conv_2d_backward(Tensor<float, 4> const &input, Tensor<float, 2> const &kernel,
                      Tensor<float, 4> const &d_output, int kernel_size, int stride)
    -> std::tuple<Tensor<float, 4>, Tensor<float, 2>>;

auto conv_2d_backward(Tensor<float, 3> const &input, Tensor<float, 2> const &kernel,
                      Tensor<float, 3> const &d_output, int kernel_size, int stride)
    -> std::tuple<Tensor<float, 3>, Tensor<float, 2>>;

auto pad(MatrixF const &matrix, int pad_row, int pad_col) -> ts::MatrixF;

auto max_pool_2d(ts::Tensor<float, 4> const &input, int kernel_size, int stride)
    -> std::pair<ts::Tensor<float, 4>, ts::Tensor<bool, 4>>;

auto max_pool_2d_backward(ts::Tensor<float, 4> const &d_output, ts::Tensor<bool, 4> const &mask,
                          int kernel_size, int stride) -> ts::Tensor<float, 4>;

} // namespace ts
