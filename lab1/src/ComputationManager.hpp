#pragma once

#include "Executor.hpp"
#include "ShortCircuitOperation.hpp"
#include "TimeMeasuringExecutor.hpp"
#include "MutualDefs.hpp"


#include <atomic>
#include <mutex>

namespace lab {

    template <typename T, typename Executor>
    concept NthResultCallbackType = requires (T x) {
        x(std::declval<std::size_t>(),
          std::declval<int>(),
          std::declval<std::chrono::milliseconds>());
    };

    template <auto Operation,
              typename T,
              typename ResultCallback,
              AsyncExecutor Executor,
              NthResultCallbackType<Executor> NthResultCallback>
    struct ManagerParams {
        ShortCircuitOperation<Operation, T> operation;
        std::vector<Executor> executors;
        NthResultCallback on_nth_result;
        ResultCallback on_result;
    };

    template <auto Operation,
              typename T,
              AsyncExecutor Executor,
              NthResultCallbackType<Executor> NthResultCallback>
    class ComputationManager {

    public:
        template <typename ResultCallback>
        explicit ComputationManager(ManagerParams<Operation, T, ResultCallback, Executor, NthResultCallback>&& params)
            : _executors {std::move(params.executors)},
              _op {std::move(params.operation)},
              _on_nth_res{std::move(params.on_nth_result)},
              _starts (_executors.size())
        {
            _op.on_result([&] (std::size_t exec_number, bool result) {
                for (std::size_t i = 0; i < _executors.size(); ++i) {
                    if (i != exec_number) {
                        _executors[i].stop();
                    }
                }
                params.on_result(result);
                _running = false;
            });

            auto count = std::size_t{0};
            for (auto& executor : _executors) {
                executor.on_success([&, n = count++](ct::return_type_t<typename std::remove_cvref_t<decltype(executor)>::Function> result) {
                    static std::mutex mut;
                    std::scoped_lock lock{mut};
                    if (!_running) {
                        return;
                    }
                    const auto a = std::chrono::steady_clock::now() - _starts[n];
                    _on_nth_res(n, result, std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - _starts[n]
                            ));
                    _op.set_operand(n, result);
                });
            }
        }

        template <rng::range Args>
        requires std::is_convertible_v<rng::range_value_t<Args>, ct::args_t<typename Executor::Function>>
        auto run (const Args& args) -> void
        {
            if (rng::size(args) != _executors.size()) {
                throw std::invalid_argument{"Incorrect number of args"};
            }
            _running = true;
            for (std::size_t i = 0; i < _executors.size(); ++i) {
                std::apply([&] (auto&&... args) {
                    _starts[i] = std::chrono::steady_clock::now();
                   _executors[i].run(std::forward<decltype(args)>(args)...);
               }, args[i]);
            }
        }

        auto stop (const std::size_t n) -> void
        {
            _executors[n].stop();
        }

        auto stop() -> void
        {
            for (auto& executor : _executors | rng::views::filter([] (const auto& ex) {return ex.running();}))
            {
                executor.stop();
            }
            _running = false;
        }

        [[nodiscard]]
        auto running() const noexcept -> bool
        {
            return _running;
        }

    ~ComputationManager() {
        for (auto& executor : _executors | rng::views::filter([] (const auto& ex) {return ex.running();})) {
            executor.stop();
        }
    }

    private:
        std::vector<Executor> _executors;
        ShortCircuitOperation<Operation, T> _op;
        NthResultCallback _on_nth_res;
        std::atomic<bool> _running = false;
        std::vector <std::chrono::steady_clock::time_point> _starts;
    };
}
