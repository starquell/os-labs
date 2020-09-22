#include "src/InThreadExecutor.hpp"
#include "src/ShortCircuitOperation.hpp"
#include <ComputationManager.hpp>
#include "MyVariant.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std::chrono_literals;

int main() {

    auto mut = std::mutex{};

    lab::ComputationManager manager {lab::ManagerParams{
        .operation = lab::ShortCircuitDisjunction{3},
        .executors = std::vector {
                lab::InThreadExecutor{lab::variant::f_func<lab::variant::operation>},
                lab::InThreadExecutor{lab::variant::g_func<lab::variant::operation>},
                lab::InThreadExecutor{lab::variant::f_func<lab::variant::operation>},
        },
        .on_nth_result = [&] (std::size_t n, int res) {
            std::scoped_lock{mut};
            std::cout << "\nOgo : " << res << " n : " << n << std::flush;
        },
        .on_result = [&] (bool b) {
            std::scoped_lock{mut};
            std::cout << "\nFinally : " << b << std::flush;
        }
    }};
    manager.run(std::vector{std::tuple{4}, std::tuple{1}, std::tuple{3}});
//    manager.stop(1);
    std::this_thread::sleep_for(40s);

    return 0;
}
