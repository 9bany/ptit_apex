#include "test_runner.hpp"
#include "apex/Bank.hpp"
#include "apex/Errors.hpp"

void suite_transfer() {
    std::cout << "[Transfer] Cung tien te\n";
    {
        Bank bank;
        // Dung checking de test chuyen thanh cong
        std::string src = bank.createChecking("An",   Money{1'000'000, Currency::VND}, 0);
        std::string dst = bank.createChecking("Binh", Money{500'000,   Currency::VND}, 0);

        EXPECT_NO_THROW("Chuyen khoan thanh cong", {
            bank.transfer(src, dst, Money{300'000, Currency::VND});
            if (bank.getAccount(src).balance().amount() != 700'000)
                throw std::runtime_error("so du nguon sai");
            if (bank.getAccount(dst).balance().amount() != 800'000)
                throw std::runtime_error("so du dich sai");
        });

        // Dung savings (khong co thu chi) de test InsufficientFunds qua transfer
        std::string src2 = bank.createSavings("Cam", Money{100'000, Currency::VND}, 0.05L, 500'000);
        EXPECT_THROW("Chuyen khoan vuot so du nguon (savings)", InsufficientFunds, {
            bank.transfer(src2, dst, Money{500'000, Currency::VND});
        });
    }

    std::cout << "[Transfer] Khac tien te\n";
    {
        Bank bank;
        std::string src = bank.createChecking("An",   Money{1'000'000, Currency::VND}, 0);
        std::string dst = bank.createChecking("Binh", Money{0,         Currency::USD}, 0);

        EXPECT_NO_THROW("Chuyen VND -> USD (500k VND = 20 USD)", {
            bank.transfer(src, dst, Money{500'000, Currency::VND});
            long double usd = bank.getAccount(dst).balance().amount();
            if (usd < 19.9L || usd > 20.1L)
                throw std::runtime_error("so du USD sai");
        });
    }

    std::cout << "[Transfer] Transaction log\n";
    {
        Bank bank;
        std::string id = bank.createChecking("An", Money{1'000'000, Currency::VND}, 0);

        bank.deposit(id, Money{200'000, Currency::VND});
        bank.withdraw(id, Money{50'000,  Currency::VND});

        EXPECT_NO_THROW("Log ghi du 2 giao dich sau deposit + withdraw", {
            auto txns = bank.log().forAccount(id);
            // AccountCreate + Deposit + Withdraw = 3
            if (txns.size() != 3)
                throw std::runtime_error("so luong giao dich sai: got " +
                                         std::to_string(txns.size()));
        });
    }

    std::cout << "[Transfer] Ty gia khong hop le\n";
    {
        Bank bank;
        EXPECT_THROW("Dat ty gia am",    InvalidRate, { bank.converter().setRate(Currency::USD, Currency::VND, -1.0L); });
        EXPECT_THROW("Dat ty gia bang 0", InvalidRate, { bank.converter().setRate(Currency::USD, Currency::VND,  0.0L); });
    }
}
