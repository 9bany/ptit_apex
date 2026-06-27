#include "test_runner.hpp"
#include "apex/Bank.hpp"
#include "apex/Errors.hpp"

void suite_savings() {
    std::cout << "[Savings] Rut het so du\n";
    {
        Bank bank;
        std::string id = bank.createSavings("An", Money{500'000, Currency::VND}, 0.05L, 500'000);

        EXPECT_NO_THROW("Rut dung het so du", {
            bank.withdraw(id, Money{500'000, Currency::VND});
            if (bank.getAccount(id).balance().amount() != 0)
                throw std::runtime_error("so du phai bang 0");
        });
        EXPECT_THROW("Rut tiep khi het tien", InsufficientFunds, {
            bank.withdraw(id, Money{1, Currency::VND});
        });
    }

    std::cout << "[Savings] Han muc rut trong ngay\n";
    {
        Bank bank;
        // So du: 1,000,000 VND; han muc ngay: 300,000 VND
        std::string id = bank.createSavings("An", Money{1'000'000, Currency::VND}, 0.05L, 300'000);

        EXPECT_NO_THROW("Rut trong han muc ngay", {
            bank.withdraw(id, Money{200'000, Currency::VND});
        });
        EXPECT_THROW("Rut vuot han muc ngay (tong 400k > 300k)", InsufficientFunds, {
            bank.withdraw(id, Money{200'000, Currency::VND});
        });
    }
}
