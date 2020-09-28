#pragma once

#include <vector>

#include <fmt/include/fmt/color.h>
#include <indicators/indeterminate_progress_bar.hpp>
#include <indicators/dynamic_progress.hpp>

#include "Cancelator.hpp"

namespace lab {

    template <Cancelator ManagerCancelator>
    class UI {

    public:
        explicit UI (ManagerCancelator&& cancelator) noexcept;

        auto run() -> void;

    private:
        [[nodiscard]]
        auto welcome() const -> std::pair<std::vector<unsigned>, std::vector<unsigned>>;

        auto main_loop (auto& manager) -> void;

        auto set_progress_bars (const std::size_t f, const std::size_t g) -> void;

        auto set_cancelator (auto& manager) -> void;

    private:
        struct Colors {
            fmt::color welcome = fmt::color::light_green;
            indicators::Color indicator = indicators::Color::yellow;
            fmt::color success = fmt::color::forest_green;
            fmt::color canceled = fmt::color::white;
        };
        Colors _colors;
        indicators::DynamicProgress<indicators::IndeterminateProgressBar> _bar;
        std::vector <indicators::IndeterminateProgressBar> _bars;
        ManagerCancelator _cancelator;
    };


}