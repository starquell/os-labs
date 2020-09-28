#pragma once

#include "Executor.hpp"

#include <chrono>

namespace lab {

    template <AsyncExecutor Executor>
    class TimeMeasuringExecutor : public Executor {

    public:
        explicit TimeMeasuringExecutor(auto&&... args)
            : Executor {std::forward<decltype(args)>(args)...}
        {}

        auto run(auto&&... args)
        {
            const auto start = std::chrono::steady_clock::now();
            Executor::run(std::forward<decltype(args)>(args)...);
            const auto finish = std::chrono::steady_clock::now();
            _last_elapsed = finish - start;
        }

        template <typename TimeInterval>
        auto last_elapsed_time () const noexcept -> TimeInterval
        {
            return std::chrono::duration_cast<TimeInterval>(_last_elapsed);
        }

    private:
        std::optional<std::chrono::steady_clock::duration> _last_elapsed;
    };
}