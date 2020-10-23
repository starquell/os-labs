#pragma once

#include <string_view>
#include <optional>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <queue>

namespace lab {

    template <typename Func>
    class FunctionExecutor {
    public:

        explicit FunctionExecutor (Func func, std::string_view name) noexcept
            : _func{func},
              _name{name}
        {}

        auto run(int arg,
                 std::condition_variable& cv,
                 std::mutex& mut,
                 std::queue<std::pair<std::string, bool>>& results) noexcept -> void
        {
            std::thread th {
                [&, arg = arg] {
                    auto result = _func(arg);
                    {
                        std::scoped_lock lock {mut};
                        results.emplace(_name, result);
                    }
                    cv.notify_one();
                }
            };
            th.detach();
        }

    private:
        Func _func;
        std::string _name;
    };
}

