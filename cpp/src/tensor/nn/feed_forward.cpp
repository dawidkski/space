#include "feed_forward.hpp"
#include "initialization.hpp"


namespace ts {

FeedForward::FeedForward(Variable<float, 2> weight, Variable<float, 1> bias, Activation activation)
    : _weight(std::move(weight)), _bias(std::move(bias)), _activation(Activations::get(activation)) {}

auto FeedForward::create(int dim_in, int dim_out, Activation activation) -> FeedForward
{
    auto weight =
        Variable<float, 2>(std::make_unique<ts::MatrixF>(ts::kaiming_uniform<float, 2>({dim_in, dim_out})),
                           std::make_unique<ts::MatrixF>(ts::kaiming_uniform<float, 2>({dim_in, dim_out})),
                           "FeedForward(weight)");

    auto bias = Variable<float, 1>(std::make_unique<ts::VectorF>(ts::bias_init<float, 1>({dim_out})),
                                   std::make_unique<ts::VectorF>(ts::bias_init<float, 1>({dim_out})),
                                   "FeedForward(bias)  ");
    return FeedForward(std::move(weight), std::move(bias), activation);
}

auto FeedForward::operator()(MatrixF const &inputs) -> MatrixF { return forward(inputs); }

auto FeedForward::forward(MatrixF const &inputs) -> MatrixF
{
    _x = inputs;
    auto _y = ts::add(ts::dot(_x, _weight.tensor()), _bias.tensor());
    if (_activation) {
        _y = _activation.value()->forward(_y);
    }
    return _y;
}

auto FeedForward::backward(MatrixF const &d_y) -> MatrixF
{
    MatrixF d_output = d_y.clone();
    if (_activation) {
        d_output = _activation.value()->backward(d_output);
    }
    _weight.grad() = ts::dot(_x, d_output, true);
    _bias.grad() = ts::sum(d_output, 0);

    return ts::dot(d_output, _weight.tensor(), false, true);
}

auto FeedForward::weight() -> Variable<float, 2> & { return _weight; }

auto FeedForward::bias() -> Variable<float, 1> & { return _bias; }

auto FeedForward::weights() -> VectorRef
{
    std::vector<std::reference_wrapper<ts::GradHolder<float>>> vars;
    vars.emplace_back(std::ref(weight()));
    vars.emplace_back(std::ref(bias()));
    return vars;
}

}
