#include "apex/Bank.hpp"
#include "apex/Menu.hpp"
#include "apex/SampleData.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    Bank bank;

    // Pre-load sample data if --demo flag is passed
    if (argc > 1 && std::string(argv[1]) == "--demo") {
        SampleData::seed(bank);
        std::cout << "\n";
    }

    Menu menu(bank);
    menu.run();
    return 0;
}
