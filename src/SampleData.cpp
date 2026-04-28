#include "apex/SampleData.hpp"
#include "apex/IdGenerator.hpp"
#include <iostream>

namespace SampleData {

void seed(Bank& bank) {
    std::cout << "\n  [*] Dang tao du lieu mau...\n";

    // Exchange rates already set in CurrencyConverter ctor — optionally refresh
    bank.converter().setRate(Currency::USD, Currency::VND, 25'500.0L);
    bank.converter().setRate(Currency::EUR, Currency::VND, 27'200.0L);

    // 5 demo accounts: mix of savings & checking, mix of currencies
    auto a1 = bank.createSavings("Nguyen Van An",
                                 Money{50'000'000.0L, Currency::VND}, 0.07L, 20'000'000.0L);
    auto a2 = bank.createChecking("Tran Thi Bich",
                                  Money{2'500.0L, Currency::USD}, 1'000.0L);
    auto a3 = bank.createSavings("Le Van Cuong",
                                 Money{1'500.0L, Currency::EUR}, 0.05L, 500.0L);
    auto a4 = bank.createChecking("Pham Thi Dung",
                                  Money{30'000'000.0L, Currency::VND}, 10'000'000.0L);
    auto a5 = bank.createSavings("Hoang Van Em",
                                 Money{5'000.0L, Currency::USD}, 0.04L, 2'000.0L);

    // Some initial transactions for demo history
    bank.deposit(a1, Money{5'000'000.0L, Currency::VND});
    bank.deposit(a2, Money{500.0L, Currency::USD});
    bank.withdraw(a4, Money{2'000'000.0L, Currency::VND});
    bank.transfer(a2, a5, Money{200.0L, Currency::USD});

    std::cout << "  [+] Da tao 5 tai khoan mau:\n"
              << "      " << a1 << " - Nguyen Van An    (Tiet kiem, VND)\n"
              << "      " << a2 << " - Tran Thi Bich    (Thanh toan, USD)\n"
              << "      " << a3 << " - Le Van Cuong     (Tiet kiem, EUR)\n"
              << "      " << a4 << " - Pham Thi Dung    (Thanh toan, VND)\n"
              << "      " << a5 << " - Hoang Van Em     (Tiet kiem, USD)\n";
}

} // namespace SampleData
