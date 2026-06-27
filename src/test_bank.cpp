#include "test_runner.hpp"
#include <iostream>

int g_passed = 0;
int g_failed = 0;

void suite_account();
void suite_savings();
void suite_checking();
void suite_transfer();

int main() {
    std::cout << "\n=== PHASE 4: KIEM THU ===\n\n";

    suite_account();
    std::cout << "\n";
    suite_savings();
    std::cout << "\n";
    suite_checking();
    std::cout << "\n";
    suite_transfer();

    std::cout << "\n--- Ket qua: " << g_passed << " pass, " << g_failed << " fail ---\n\n";
    return g_failed == 0 ? 0 : 1;
}
