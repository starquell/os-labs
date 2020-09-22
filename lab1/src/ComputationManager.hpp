#pragma once

#include "Executor.hpp"

#include <atomic>
#include <iostream>

namespace lab {

    template <auto Operation,
              typename T,
              typename NthResultCallback,
              typename ResultCallback,
              AsyncExecutor Executor>
    struct ManagerParams {
        ShortCircuitOperation<Operation, T> operation;
        std::vector<Executor> executors;
        NthResultCallback on_nth_result;
        ResultCallback on_result;
    };

    template <auto Operation,
              typename T,
              typename NthResultCallback,
              AsyncExecutor Executor>
    class ComputationManager {
    public:

        template <typename ResultCallback>
        explicit ComputationManager(ManagerParams<Operation, T, NthResultCallback, ResultCallback, Executor>&& params)
            : _executors {std::move(params.executors)},
              _op {std::move(params.operation)},
              _on_nth_res{std::move(params.on_nth_result)}
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
                    _on_nth_res(n, result);
                    _op.set_operand(n, result);
                });
            }
        }

        template <rng::range Args>
        requires std::is_same_v<rng::range_value_t<Args>, ct::args_t<typename Executor::Function>>
        auto run (const Args& args) -> void
        {
            if (rng::size(args) != _executors.size()) {
                throw std::invalid_argument{"Incorrect number of args"};
            }
            _running = true;
            for (std::size_t i = 0; i < _executors.size(); ++i) {
                std::apply([&] (auto&&... args) {
                   _executors[i].run(std::forward<decltype(args)>(args)...);
               }, args[i]);
            }
        }

        auto stop (const std::size_t n) -> void
        {
            _executors[n].stop();
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
    };
}
