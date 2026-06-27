#include "test_runner.hpp"
#include "apex/Bank.hpp"
#include "apex/Errors.hpp"

void suite_account() {
    std::cout << "[Account] Tao tai khoan & truy cap\n";
    {
        Bank bank;
        std::string sid = bank.createSavings("An", Money{1'000'000, Currency::VND}, 0.05L, 500'000);
        std::string cid = bank.createChecking("Binh", Money{2'000'000, Currency::VND}, 500'000);

        EXPECT_NO_THROW("Tao savings: owner va so du dung", {
            Account& a = bank.getAccount(sid);
            if (a.owner() != "An") throw std::runtime_error("owner sai");
            if (a.balance().amount() != 1'000'000) throw std::runtime_error("so du sai");
        });
        EXPECT_NO_THROW("Tao checking: owner dung", {
            Account& a = bank.getAccount(cid);
            if (a.owner() != "Binh") throw std::runtime_error("owner sai");
        });
        EXPECT_THROW("getAccount ID khong ton tai", AccountNotFound, {
            bank.getAccount("XXX-999");
        });
    }

    std::cout << "[Account] Sai tien te\n";
    {
        Bank bank;
        std::string id = bank.createSavings("An", Money{1'000'000, Currency::VND}, 0.05L, 500'000);

        EXPECT_THROW("Nap sai tien te (USD vao VND)", CurrencyMismatch, {
            bank.deposit(id, Money{100, Currency::USD});
        });
        EXPECT_THROW("Rut sai tien te (USD tu VND)", CurrencyMismatch, {
            bank.withdraw(id, Money{100, Currency::USD});
        });
    }

    std::cout << "[Account] So tien khong hop le\n";
    {
        Bank bank;
        std::string id = bank.createSavings("An", Money{1'000'000, Currency::VND}, 0.05L, 500'000);

        EXPECT_THROW("Nap so tien = 0",   InvalidInput, { bank.deposit(id,  Money{0,    Currency::VND}); });
        EXPECT_THROW("Nap so tien am",    InvalidInput, { bank.deposit(id,  Money{-100, Currency::VND}); });
        EXPECT_THROW("Rut so tien = 0",   InvalidInput, { bank.withdraw(id, Money{0,    Currency::VND}); });
        EXPECT_THROW("Rut so tien am",    InvalidInput, { bank.withdraw(id, Money{-100, Currency::VND}); });
    }
}
