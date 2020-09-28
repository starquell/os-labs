#pragma once

#include <concepts>

namespace lab {

    /**
     *  @brief Abstract helper class for Executor classes
     */
    template <typename SuccessHandler, typename FailHandler>
    class AsyncExecutorBase {

    public:
        using SuccessHandlerType = SuccessHandler;
        using FailHandlerType = FailHandler;

        /**
         *  @brief Setter for callback that will be invoked after succesful executing
         */
        auto on_success(const SuccessHandler& callback) -> void
        {
            _on_success = callback;
        }

        /**
         *  @brief Setter for callback that will be invoked after succesful executing
         */
        auto on_success(SuccessHandler&& callback) noexcept -> void
        {
            _on_success = std::move(callback);
        }

        /**
         *  @brief Setter for callback that will be invoked after some error that interrupted function execution
         */
        auto on_fail(const FailHandler& callback) -> void
        {
            _on_fail = callback;
        }

         /**
         *  @brief Setter for callback that will be invoked after some error that interrupted function execution
         */
        auto on_fail(FailHandler&& callback) noexcept -> void
        {
            _on_fail = std::move(callback);
        }

        [[nodiscard]]
        auto success_handler() -> const SuccessHandlerType&
        {
            return _on_success;
        }

        [[nodiscard]]
        auto fail_handler () -> const FailHandlerType&
        {
            return _on_fail;
        }

    private:
        SuccessHandler _on_success;
        FailHandler _on_fail;
    };

    template <typename T>
    concept AsyncExecutor =
    requires (T x) {
        x.run();
        {x.running()} -> std::convertible_to<bool>;
        x.stop();
        typename T::SuccessHandlerType;
        typename T::FailHandlerType;
    } && std::derived_from <T, AsyncExecutorBase<typename T::SuccessHandlerType, typename T::FailHandlerType>>;
}