#pragma once

#include <algorithm>
#include <ranges>

namespace lab::operation {

     struct Disjuction {

         static constexpr auto compute (std::ranges::range auto ops) -> bool
         {
            return std::ranges::any_of(ops, [] (bool v) {return v;});
         };

         static auto name() -> std::string
         {
             return "disjunction";
         }

         static auto is_short_circuit(auto value) -> bool
         {
             return value;
         }
     };

     struct Conjunction {

          static constexpr auto compute (std::ranges::range auto ops) -> bool
         {
            return std::ranges::all_of(ops, [] (bool v) {return v;});
         };

         static auto name() -> std::string
         {
             return "conjunction";
         }

         static auto is_short_circuit(auto value) -> bool
         {
             return !value;
         }
     };
}

