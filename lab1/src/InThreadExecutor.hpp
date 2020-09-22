#pragma once

#include "Utils.hpp"
#include "Executor.hpp"

#include <functional>
#include <string_view>
#include <optional>
#include <thread>

#include <boost/callable_traits/return_type.hpp>

namespace ct = boost::callable_traits;
namespace rng = std::ranges;

namespace lab {

    /**
     *  @brief Represents function executor, execution will be in separate thread
     *         If thread launchs normally and function will not throw any exception, callback set via on_result method will be called,
     *         otherwise - callback set via on_fail method
     */
    template <typename Func>
    class InThreadExecutor: public AsyncExecutorBase<
            std::function<void(ct::return_type_t<Func>)>,
            std::function<void(std::string_view)>>{
    public:
        using Function = Func;

        explicit InThreadExecutor(Func func) noexcept;

        InThreadExecutor(const InThreadExecutor& other);

        /**
         *  @brief Call func in separate thread with args as arguments
         */
        auto run(auto... args) noexcept -> void;

        /**
         *  @brief Blocks thread that call this method until function will not be executed and callback called
         */
        auto wait() noexcept -> void;

        /**
         *  @brief After this method call result of execution will not be passed to any set callback, execution thread will be detached
         */
        auto stop() -> void;

        /**
         *  @brief Returns true if run have been called, function hasn't finish exection and stop() hasn't been called
         */
        [[nodiscard]]
        auto running() const noexcept -> bool;

    private:
        Func _func;
        std::optional<std::jthread> _thread;
        std::atomic<bool> _running = false;
    };

    template <typename Func>
    InThreadExecutor (Func) -> InThreadExecutor<Func>;
}

#include "InThreadExecutor.hxx"