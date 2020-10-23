#pragma once

#include <demofuncs.hpp>

#include <algorithm>
#include <ranges>

namespace lab::variant {

    constexpr auto f_func = spos::lab1::demo::f_func<spos::lab1::demo::op_group::OR>;
    constexpr auto g_func = spos::lab1::demo::g_func<spos::lab1::demo::op_group::OR>;

    /// Disjunction
    constexpr auto operation = [] (std::ranges::range auto ops) {
        return std::ranges::any_of(ops, [] (bool v) {return v == true;});
    };


}