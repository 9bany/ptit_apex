#pragma once
#include "apex/Account.hpp"

// Tai khoan tiet kiem: co lai suat va han muc rut tien trong ngay.
class SavingsAccount : public Account {
public:
    SavingsAccount(const std::string& id, const std::string& owner,
                   const Money& initialBalance,
                   long double interestRate, long double dailyWithdrawCap);

    std::string type() const override { return "Tiet kiem"; }
    void applyInterest();
    void display(std::ostream& os) const override;

protected:
    void checkWithdrawRules(const Money& m) override;
    void onAfterWithdraw(const Money& m) override;

private:
    long double interestRate_;
    long double dailyWithdrawCap_;
    long double withdrawnToday_;
};
