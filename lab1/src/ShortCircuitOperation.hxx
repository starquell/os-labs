#pragma once

#include "ShortCircuitOperation.hpp"

template <auto Op, std::equality_comparable T>
lab::ShortCircuitOperation<Op, T>::ShortCircuitOperation(
        const std::size_t arity,
        const T& short_circuit_value)
        : _operands(arity),
          _short_circuit_value{short_circuit_value}
{}

template <auto Op, std::equality_comparable T>
auto lab::ShortCircuitOperation<Op, T>::on_result(const CallbackType& callback) -> void
{
    _callback = callback;
}

template <auto Op, std::equality_comparable T>
auto lab::ShortCircuitOperation<Op, T>::on_result(CallbackType&& callback) noexcept -> void
{
    _callback = std::move(callback);
}

template <auto Op, std::equality_comparable T>
auto lab::ShortCircuitOperation<Op, T>::set_operand(const std::size_t n, const T& value) -> void
{
    if (value == _short_circuit_value) {
        _callback(n, Op(std::vector<std::optional<T>>{value}));
    }
    else {
        if (!_operands[n]) {
            _operands[n] = value;
            ++_checked;
            if (_checked == _operands.size()) {
                _callback(n, Op(_operands));
            }
        }
    }
}

template <auto Op, std::equality_comparable T>
auto lab::ShortCircuitOperation<Op, T>::arity() const noexcept -> std::size_t
{
    return _operands.size();
}

