#include "CLI.hpp"

#include "ComputationManager.hpp"
#include "InThreadExecutor.hpp"
#include "../MyVariant.hpp"
#include "MutualDefs.hpp"

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <indicators/cursor_control.hpp>

#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

namespace {
     auto indicator_info (const std::size_t f_funcs, const std::size_t g_funcs, const std::size_t n) -> std::string
     {
        if (n >= f_funcs) {
            return fmt::format("Function {} #{}", "G", n - f_funcs + 1);
        }
        else {
            return fmt::format("Function {} #{}", "F", n + 1);
        }
     };
}
namespace lab {

    template <Cancelator C>
    UI<C>::UI(C&& cancelator) noexcept
        : _cancelator (std::move(cancelator))
    {}

    template <Cancelator C>
    auto UI<C>::run() -> void
    {
        auto args = welcome();
        auto executors = [&] {
            std::vector res (args.first.size(), InThreadExecutor{variant::f_func<variant::operation>});
            res.insert(res.end(), args.second.size(), InThreadExecutor{variant::g_func<variant::operation>});
            return res;
        }();
        const auto f_number = args.first.size();
        const auto g_number = args.second.size();

        set_progress_bars(f_number, g_number);

        auto manager = ComputationManager {
            ManagerParams {
                .operation = lab::ShortCircuitDisjunction{f_number + g_number},
                .executors = std::move(executors),
                .on_nth_result = [&] (std::size_t n, int res, std::chrono::milliseconds elapsed) {
                    _bar[n].set_option(indicators::option::PostfixText{
                            fmt::format("{} - completed with result {} in {}",
                                        indicator_info(f_number, g_number, n),
                                        res,
                                        std::chrono::duration_cast<std::chrono::seconds>(elapsed))});
                    _bar[n].set_option(indicators::option::ForegroundColor{indicators::Color::green});
                    _bar[n].mark_as_completed();
                },
                .on_result = [&] (bool b) {
                    fmt::print(fmt::fg(_colors.success) | fmt::emphasis::bold,
                               "\nComputation successfully finished with result ");
                    fmt::print(fmt::fg(fmt::color::light_sky_blue) | fmt::emphasis::bold, "{}\n\n", b);
                }
            }
        };
        manager.run([&] {
            std::vector <std::tuple<unsigned>> res;
            res.reserve(f_number + g_number);
            rng::for_each (args.first, [&] (const auto arg) {res.push_back(std::tuple{arg});});
            rng::for_each (args.second, [&] (const auto arg) {res.push_back(std::tuple{arg});});
            return res;
        }());

        set_cancelator(manager);
        _cancelator.start_monitoring();
        main_loop(manager);
    }

    template <Cancelator C>
    auto UI<C>::main_loop (auto& manager) -> void
    {
        fmt::print("Press 'q' to cancel computation.\n\n");

        while (manager.running()) {
            for (std::size_t i = 0; i < _bars.size(); ++i) {
                if (!_bar[i].is_completed()) {
                    _bar[i].tick();
                }
            }
            std::this_thread::sleep_for(100ms);
        }
        if (_cancelator.canceled()) {
            fmt::print(fg(_colors.canceled), "\nComputation has been stoped.\n");
        }
        else {
            _cancelator.stop_monitoring();
        }
    }

    template <Cancelator C>
    auto UI<C>::welcome() const -> std::pair<std::vector<unsigned>, std::vector<unsigned>>
    {
        fmt::print(fmt::fg(_colors.welcome),
                   "\nHello! This application used to compute result of disjunction among results of functions F and G!\n\n");
        fmt::print("Please, choose amount of F functions to be computed:\t");

        auto f_funcs = 1u;
        std::cin >> f_funcs;

        fmt::print("Also, choose amount of G functions to be computed:\t");

        auto g_funcs = 1u;
        std::cin >> g_funcs;

        fmt::print("Enter arguments for F functions separating by spaces: ");
        std::vector<unsigned> f_args (f_funcs);
        for (auto i = 0; i < f_funcs; ++i) {
            std::cin >> f_args[i];
        }

        fmt::print("Enter arguments for G functions separating by spaces: ");
        std::vector<unsigned> g_args (g_funcs);
        for (auto i = 0; i < g_funcs; ++i) {
            std::cin >> g_args[i];
        }
        fmt::print("\n");

        return {f_args, g_args};
    }

    template <Cancelator C>
    auto UI<C>::set_progress_bars(const std::size_t f, const std::size_t g) -> void
    {
        using namespace indicators;

        _bars.reserve(f + g);

        for (std::size_t i = 0; i < f + g; ++i) {
            _bars.push_back(IndeterminateProgressBar {
                option::BarWidth{terminal_width() / 8 * 3},
                option::Start{"["},
                option::Fill{"."},
                option::Lead{"<==>"},
                option::End{"]"},
                option::ForegroundColor {_colors.indicator},
                option::PostfixText {indicator_info(f, g, i)},
                option::FontStyles {std::vector{FontStyle::bold}}
            });
        }
        for (auto& bar : _bars) {
            _bar.push_back(bar);
        }
        _bar.set_option(option::HideBarWhenComplete{false});
    }

    template <Cancelator C>
    auto UI<C>::set_cancelator(auto& manager) -> void {
        _cancelator.on_cancel([&] {
            _cancelator.stop_monitoring();
            manager.stop();
        });
    }

    template class UI<SimpleKeyCancelator>;
}