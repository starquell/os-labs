#include "ComputationManager.hpp"
#include "MyVariant.hpp"
#include "Cancelator.hpp"
#include "Operations.hpp"

int main() {

    constexpr auto operation = lab::operation::Conjunction{};

    auto manager = lab::ComputationManager {
        std::vector{
            lab::FunctionExecutor{lab::variant::f_func, "F"},
            lab::FunctionExecutor{lab::variant::g_func, "G"}
        },
        operation,
        lab::SimpleKeyCancelator{'q'}
    };

    std::cout << std::string{"\nHello! This application used to compute "}
                 + decltype(operation)::name() + " among results of functions F and G!\n"
                 "Enter argument for functions : " << std::flush;
    int arg;
    std::cin >> arg;

    manager.run(arg);
    return 0;
}
