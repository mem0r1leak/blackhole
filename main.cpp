#include <iostream>
#include <thread>

#include "terminal.h"

using namespace std::chrono_literals;

int main(const int argc, const char *const argv[]) {
    if (argc < 3) {
        std::cout << "Usage: bhole [suckit|belch] <filename>\n";
        return 1;
    }

    const std::string_view action = argv[1];
    const std::string_view target = argv[2];

    if (action == "suckit") {
        std::cout << "(Sucking) " << target << " into the singularity...\n";
    } else if (action == "belch") {
        std::cout << "(Belching) " << target << " back to reality...\n";
    } else {
        std::cerr << "Error: Black hole doesn't know how to '" << action << "'\n";
        return 1;
    }

    return 0;
}
