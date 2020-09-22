#pragma once

#include <optional>
#include <concepts>
#include <functional>
#include <algorithm>
#include <vector>
#include <ranges>
#include <mutex>

namespace lab {

    /**
    *  @brief Class represents operation with short-circuit evaluation among N operands
    *  @param Operation functor used as operation among N operands
     * @param T operand type
    */
    template <auto Operation, std::equality_comparable T>
    class ShortCircuitOperation {

    public:
        using OperandType = T;
        using CallbackType = std::function<void(std::size_t, bool)>;

        explicit ShortCircuitOperation () noexcept = default;
        explicit ShortCircuitOperation (const std::size_t arity,
                                        const T& short_circuit_value);
        ShortCircuitOperation (ShortCircuitOperation&& other) noexcept
            : _operands {std::move(other._operands)},
              _checked {other._checked},
              _short_circuit_value (other._short_circuit_value),
              _callback(other._callback)
        {}

        /**
         *  @brief Sets callback function, that will be invoked straight after disjunction result will be known
         */
        auto on_result (const CallbackType& callback) -> void;
        auto on_result (CallbackType&& callback) noexcept -> void;

        /**
         *  @brief Sets value of nth operand of disjunction
         *         If value represents logical false, result of disjunction will be known
         */
        auto set_operand (const std::size_t n, const T& value) -> void;

        [[nodiscard]]
        auto arity() const noexcept -> std::size_t;

    private:
        std::vector<std::optional<T>> _operands;
        std::size_t _checked = 0;
        T _short_circuit_value;
        CallbackType _callback;
    };

    namespace detail {
        constexpr auto disjunction = [] (const rng::range auto& ops) -> bool
        {
            return std::ranges::any_of(ops, [] (const auto& el) -> bool {return *el;});
        };
    }

    class ShortCircuitDisjunction : public ShortCircuitOperation<
                                            detail::disjunction,
                                            bool> {
    public:
        explicit ShortCircuitDisjunction (const std::size_t arity)
            : ShortCircuitOperation<detail::disjunction, bool> {arity, true}
        {}
    };


}

#include "ShortCircuitOperation.hxx"