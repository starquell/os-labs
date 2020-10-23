#include "ComputationManager.hpp"
#include "MyVariant.hpp"
#include "Cancelator.hpp"

int main() {

    auto manager = lab::ComputationManager {
        std::vector{
                lab::FunctionExecutor{lab::variant::f_func, "F"},
                lab::FunctionExecutor{lab::variant::g_func, "G"}
        },
        lab::variant::operation,
        true,
        lab::SimpleKeyCancelator{'q'}
    };

    std::cout << "\nHello! This application used to compute result of disjunction among results of functions F and G!\n"
                 "Enter argument for functions : " << std::flush;
    int arg;
    std::cin >> arg;

    manager.run(arg);
    return 0;
}
