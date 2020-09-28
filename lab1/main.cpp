#include "CLI.hpp"

int main() {

    lab::UI ui {lab::SimpleKeyCancelator{'q'}};
    ui.run();

    return 0;
}
