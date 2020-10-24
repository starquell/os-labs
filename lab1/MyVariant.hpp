#pragma once

#include <demofuncs.hpp>

namespace lab::variant {

    constexpr auto f_func = spos::lab1::demo::f_func<spos::lab1::demo::op_group::OR>;
    constexpr auto g_func = spos::lab1::demo::g_func<spos::lab1::demo::op_group::OR>;

}