#include "apex/Bank.hpp"
#include "apex/Errors.hpp"
#include "apex/Money.hpp"
#include <iostream>
#include <string>

// --- Mini test runner ---
static int passed = 0;
static int failed = 0;

static void ok(const std::string& name) {
    std::cout << "  [PASS] " << name << "\n";
    ++passed;
}

static void fail(const std::string& name, const std::string& reason) {
    std::cout << "  [FAIL] " << name << " — " << reason << "\n";
    ++failed;
}

// Macro: ky vong block NEM exception kieu T
#define EXPECT_THROW(name, T, block)        \
    try {                                   \
        block;                              \
        fail(name, "khong nem exception");  \
    } catch (const T&) {                    \
        ok(name);                           \
    } catch (...) {                         \
        fail(name, "sai loai exception");   \
    }

// Macro: ky vong block KHONG nem exception
#define EXPECT_NO_THROW(name, block)                        \
    try {                                                   \
        block;                                              \
        ok(name);                                           \
    } catch (const std::exception& e) {                     \
        fail(name, std::string("nem ngoai le: ") + e.what()); \
    }

// --- Test cases ---

void test_create_and_display() {
    Bank bank;
    std::string sid = bank.createSavings("An", Money{1'000'000, Currency::VND}, 0.05L, 500'000);
    std::string cid = bank.createChecking("Binh", Money{2'000'000, Currency::VND}, 500'000);

    EXPECT_NO_THROW("Tao tai khoan savings", {
        Account& a = bank.getAccount(sid);
        if (a.owner() != "An") throw std::runtime_error("owner sai");
        if (a.balance().amount() != 1'000'000) throw std::runtime_error("so du sai");
    });

    EXPECT_NO_THROW("Tao tai khoan checking", {
        Account& a = bank.getAccount(cid);
        if (a.owner() != "Binh") throw std::runtime_error("owner sai");
    });

    EXPECT_THROW("getAccount ID khong ton tai", AccountNotFound, {
        bank.getAccount("XXX-999");
    });
}

void test_currency_mismatch() {
    Bank bank;
    std::string id = bank.createSavings("An", Money{1'000'000, Currency::VND}, 0.05L, 500'000);

    EXPECT_THROW("Nap tien sai tien te (USD vao VND)", CurrencyMismatch, {
        bank.deposit(id, Money{100, Currency::USD});
    });

    EXPECT_THROW("Rut tien sai tien te (USD tu VND)", CurrencyMismatch, {
        bank.withdraw(id, Money{100, Currency::USD});
    });
}

void test_invalid_amount() {
    Bank bank;
    std::string id = bank.createSavings("An", Money{1'000'000, Currency::VND}, 0.05L, 500'000);

    EXPECT_THROW("Nap so tien = 0", InvalidInput, {
        bank.deposit(id, Money{0, Currency::VND});
    });

    EXPECT_THROW("Nap so tien am", InvalidInput, {
        bank.deposit(id, Money{-100, Currency::VND});
    });

    EXPECT_THROW("Rut so tien = 0", InvalidInput, {
        bank.withdraw(id, Money{0, Currency::VND});
    });
}

void test_savings_withdraw_full_balance() {
    Bank bank;
    std::string id = bank.createSavings("An", Money{500'000, Currency::VND}, 0.05L, 500'000);

    EXPECT_NO_THROW("Rut dung het so du savings", {
        bank.withdraw(id, Money{500'000, Currency::VND});
        if (bank.getAccount(id).balance().amount() != 0)
            throw std::runtime_error("so du phai bang 0");
    });
}

void test_savings_daily_cap() {
    Bank bank;
    // Han muc rut ngay: 300,000 VND; so du: 1,000,000 VND
    std::string id = bank.createSavings("An", Money{1'000'000, Currency::VND}, 0.05L, 300'000);

    EXPECT_NO_THROW("Rut trong han muc ngay", {
        bank.withdraw(id, Money{200'000, Currency::VND});
    });

    EXPECT_THROW("Rut vuot han muc ngay", InsufficientFunds, {
        bank.withdraw(id, Money{200'000, Currency::VND}); // tong: 400,000 > 300,000
    });
}

void test_checking_overdraft() {
    Bank bank;
    // So du: 100,000 VND; han muc thu chi: 200,000 VND
    std::string id = bank.createChecking("Binh", Money{100'000, Currency::VND}, 200'000);

    EXPECT_NO_THROW("Rut vuot so du nhung trong han muc thu chi", {
        bank.withdraw(id, Money{250'000, Currency::VND}); // so du: -150,000 > -200,000: ok
    });

    EXPECT_THROW("Rut vuot ca han muc thu chi", OverdraftExceeded, {
        bank.withdraw(id, Money{100'000, Currency::VND}); // so du: -250,000 < -200,000: loi
    });
}

void test_transfer_same_currency() {
    Bank bank;
    std::string src = bank.createChecking("An",   Money{1'000'000, Currency::VND}, 0);
    std::string dst = bank.createChecking("Binh", Money{500'000,   Currency::VND}, 0);

    EXPECT_NO_THROW("Chuyen khoan cung tien te", {
        bank.transfer(src, dst, Money{300'000, Currency::VND});
        if (bank.getAccount(src).balance().amount() != 700'000)
            throw std::runtime_error("so du nguon sai");
        if (bank.getAccount(dst).balance().amount() != 800'000)
            throw std::runtime_error("so du dich sai");
    });
}

void test_transfer_cross_currency() {
    Bank bank;
    std::string src = bank.createChecking("An",   Money{1'000'000, Currency::VND}, 0);
    std::string dst = bank.createChecking("Binh", Money{0,         Currency::USD}, 0);

    EXPECT_NO_THROW("Chuyen khoan khac tien te (VND -> USD)", {
        bank.transfer(src, dst, Money{500'000, Currency::VND});
        // 500,000 VND / 25,000 = 20 USD
        long double usd = bank.getAccount(dst).balance().amount();
        if (usd < 19.9L || usd > 20.1L)
            throw std::runtime_error("so du USD sau chuyen khoan sai");
    });
}

void test_negative_rate() {
    Bank bank;
    EXPECT_THROW("Dat ty gia am", InvalidRate, {
        bank.converter().setRate(Currency::USD, Currency::VND, -1.0L);
    });
    EXPECT_THROW("Dat ty gia bang 0", InvalidRate, {
        bank.converter().setRate(Currency::USD, Currency::VND, 0.0L);
    });
}

// --- Main ---
int main() {
    std::cout << "\n=== PHASE 4: KIEM THU ===\n\n";

    std::cout << "[1] Tao tai khoan & truy cap\n";
    test_create_and_display();

    std::cout << "\n[2] Sai tien te\n";
    test_currency_mismatch();

    std::cout << "\n[3] So tien khong hop le\n";
    test_invalid_amount();

    std::cout << "\n[4] Savings: rut het so du\n";
    test_savings_withdraw_full_balance();

    std::cout << "\n[5] Savings: han muc rut trong ngay\n";
    test_savings_daily_cap();

    std::cout << "\n[6] Checking: han muc thu chi\n";
    test_checking_overdraft();

    std::cout << "\n[7] Chuyen khoan cung tien te\n";
    test_transfer_same_currency();

    std::cout << "\n[8] Chuyen khoan khac tien te\n";
    test_transfer_cross_currency();

    std::cout << "\n[9] Ty gia am / bang 0\n";
    test_negative_rate();

    std::cout << "\n--- Ket qua: " << passed << " pass, " << failed << " fail ---\n\n";
    return failed == 0 ? 0 : 1;
}
