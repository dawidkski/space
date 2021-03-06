#pragma once
#include "tensor_forward.hpp"
#include <cassert>
#include <functional>

namespace ts {

template <typename Element> using Fn = std::function<Element(Element)>;

template <typename Element, int Dim>
auto add_(Tensor<Element, Dim> const &, Tensor<Element, Dim> const &) -> void;

template <typename Element, int Dim>
auto add(Tensor<Element, Dim> const &, Tensor<Element, Dim> const &) -> Tensor<Element, Dim>;

template <typename Element>
auto add(Tensor<Element, 2> const &, Tensor<Element, 1> const &) -> Tensor<Element, 2>;

template <typename Element>
auto add(Tensor<Element, 3> const &, Tensor<Element, 1> const &) -> Tensor<Element, 3>;

template <typename Element>
auto add_(Tensor<Element, 3> const &, Tensor<Element, 1> const &) -> void;

auto divide(MatrixF const &, VectorF const &) -> MatrixF;

template <typename Element, int Dim>
auto maximum(Element, Tensor<Element, Dim> const &) -> Tensor<Element, Dim>;

template <typename Element, int Dim>
auto mask(Tensor<Element, Dim> const &, std::function<bool(Element)>) -> Tensor<bool, Dim>;

template <typename Element, int Dim>
auto assign_if(Tensor<Element, Dim> const &, Tensor<bool, Dim> const &, Element)
    -> Tensor<Element, Dim>;

template <typename Element, int Dim>
auto apply_if(Tensor<Element, Dim>, Tensor<bool, Dim>, Fn<Element>) -> Tensor<Element, Dim>;

template <typename Element, int Dim>
auto multiply(Tensor<Element, Dim> const &, Element) -> Tensor<Element, Dim>;

template <typename Element, int Dim>
auto multiply(Tensor<Element, Dim> const &, Tensor<Element, Dim> const &) -> Tensor<Element, Dim>;

auto transpose(MatrixF const &) -> MatrixF;

template <typename Element, int Dim> auto sum(Tensor<Element, Dim> const &) -> Element;

auto sum(MatrixF const &, int) -> VectorF;

auto sum_v2(MatrixF const &, int) -> VectorF;

auto to_one_hot(Tensor<int, 1> const &) -> Tensor<bool, 2>;

auto get(MatrixF const &, Tensor<int, 1> const &) -> VectorF;

template <typename Element, int Dim>
auto apply(Tensor<Element, Dim> const &, Fn<Element>) -> Tensor<Element, Dim>;

template <typename Element, int Dim> auto log(Tensor<Element, Dim> const &) -> Tensor<Element, Dim>;

template <typename Element, int Dim>
auto pow(Tensor<Element, Dim> const &tensor, int) -> Tensor<Element, Dim>;

template <typename Element, int Dim>
auto exp(Tensor<Element, Dim> const &tensor) -> Tensor<Element, Dim>;

template <int Dim>
auto randint(int low, int high, const std::vector<int> &shape) -> Tensor<int, Dim>;

template <typename Element> auto from_vector(std::vector<Element>) -> Tensor<Element, 1>;

template <typename Element> auto argmax(Tensor<Element, 2> const &tensor) -> Tensor<int, 1>;

template <typename Element, int axis>
auto concatenate(std::vector<Tensor<Element, 1>>) -> decltype(auto);

// for some unknown reasons this couldn't be in .cpp file :(
template <typename Element, int axis>
auto concatenate(std::vector<Tensor<Element, 1>> list) -> decltype(auto)
{
    if constexpr (axis == 1) {
        int vector_size = list[0].shape(0);
        Tensor<Element, 2> tensor(vector_size, list.size());
        for (int i = 0; i < vector_size; ++i) {
            for (int j = 0; j < list.size(); ++j) {
                tensor(i, j) = list[j][i];
            }
        }
        return tensor;
    } else if constexpr (axis == 0) {
        int vector_size = 0;
        for (auto const &v : list) {
            vector_size += v.shape(0);
        }

        Tensor<Element, 1> tensor(vector_size);
        int offset = 0;
        for (auto const &v : list) {
            std::copy(v.begin(), v.end(), tensor.begin() + offset);
            offset += v.shape(0);
        }
        return tensor;
    } else {
        assert(false);
    }
}

template <typename Element>
auto slice(Tensor<Element, 2> tensor, int from, int to) -> Tensor<Element, 2>
{
    std::array<int, 2> shape(tensor.shape());
    shape[0] = to - from;
    int row_size = tensor.shape(1);

    Tensor<Element, 2> slice(shape);
    int begin_offset = from * row_size;
    int end_offset = to * row_size;
    std::copy(tensor.begin() + begin_offset, tensor.begin() + end_offset, slice.begin());

    return slice;
}

template <typename Element>
auto slice(Tensor<Element, 1> tensor, int from, int to) -> Tensor<Element, 1>
{
    std::array<int, 1> shape(tensor.shape());
    shape[0] = to - from;
    int row_size = 1;

    Tensor<Element, 1> slice(shape);
    int begin_offset = from * row_size;
    int end_offset = to * row_size;
    std::copy(tensor.begin() + begin_offset, tensor.begin() + end_offset, slice.begin());

    return slice;
}

template <typename T> auto swap(T &t1, T &t2)
{
    auto temp(std::move(t1));
    t1 = std::move(t2);
    t2 = std::move(temp);
}

template <typename Element> auto clip_(DataHolder<Element> &data, Element min, Element max) -> void
{
    std::transform(data.begin(), data.end(), data.begin(), [min, max](auto &value) {
        if (value < min)
            return min;
        else if (value > max)
            return max;
        else
            return value;
    });
}

template <typename Element, int Dim>
auto clip_max_(Tensor<Element, Dim> &tensor, Element max) -> void
{
    std::transform(tensor.begin(), tensor.end(), tensor.begin(),
                   [max](auto &value) { return value > max ? max : value; });
}

template <typename Element, int Dim>
auto clip_min_(Tensor<Element, Dim> &tensor, Element min) -> void
{
    std::transform(tensor.begin(), tensor.end(), tensor.begin(),
                   [min](auto &value) { return value < min ? min : value; });
}

template <typename Element, int Dim> auto add_(Tensor<Element, Dim> &tensor, Element value) -> void
{
    std::transform(tensor.begin(), tensor.end(), tensor.begin(),
                   [value](Element e) { return e + value; });
}

template <typename Element, int Dim>
auto subtract_(Tensor<Element, Dim> const &tensor, Element value) -> void
{
    std::transform(tensor.begin(), tensor.end(), tensor.begin(),
                   [value](Element e) { return e - value; });
}

} // namespace ts