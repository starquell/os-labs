#pragma once

#include "FunctionExecutor.hpp"

#include <iostream>
#include <mutex>

namespace lab {

    template <typename Operation,
              typename Func,
              typename Cancelator>
    class ComputationManager {

    public:
        explicit ComputationManager(const std::vector<FunctionExecutor<Func>>& funcs,
                                    Operation op,
                                    Cancelator cancelator)
            : _funcs{funcs},
              _op{op},
              _cancelator{std::move(cancelator)}
        {}

        auto run (int arg) -> void
        {
            std::mutex mut;
            std::condition_variable cv;
            std::queue<std::pair<std::string, bool>> func_results;
            std::deque<bool> operands;
            std::size_t received{};

            _cancelator.start_monitoring(cv, mut);
            std::cout << "\nPress '" << static_cast<char>(_cancelator.key()) << "' to cancel computation\n\n" << std::flush;

            for (std::size_t i = 0; i < _funcs.size(); ++i) {
                _funcs[i].run(arg, cv, mut, func_results);
            }

            while (true) {
                std::unique_lock lock{mut};
                cv.wait(lock, [&] { return !func_results.empty() || _cancelator.canceled();});

                if (_cancelator.canceled()) {
                    std::cout << "Computation has been cancelled." << std::endl;
                    return;
                }

                while (!func_results.empty()) {
                    const auto [func, res] = func_results.front();
                    std::cout << "Function " << func << " returned " << res << std::endl;
                    if (_op.is_short_circuit(res)) {
                        std::cout << "\nOperation result : " << res << std::endl;
                        return;
                    }
                    operands.push_back(res);
                    ++received;
                    if (received == _funcs.size()) {
                        std::cout << "Result : " << _op.compute(operands) << std::endl;
                        return;
                    }
                    func_results.pop();
                }
            }
        }

        ~ComputationManager() {
            _cancelator.stop_monitoring();
        }

    private:
        std::vector<FunctionExecutor<Func>> _funcs;
        Operation _op;
        Cancelator _cancelator;
    };
}
