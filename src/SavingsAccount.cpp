#include "apex/SavingsAccount.hpp"
#include "apex/Errors.hpp"
#include <iostream>
#include <iomanip>

SavingsAccount::SavingsAccount(const std::string& id, const std::string& owner,
                               const Money& initialBalance,
                               long double interestRate, long double dailyWithdrawCap)
    : Account(id, owner, initialBalance),
      interestRate_(interestRate),
      dailyWithdrawCap_(dailyWithdrawCap),
      withdrawnToday_(0.0L) {}

void SavingsAccount::doWithdraw(const Money& m) {
    if (m.currency() != balance_.currency())
        throw CurrencyMismatch("Currency mismatch");
    if (m.amount() <= 0.0L)
        throw InvalidInput("Withdraw amount must be positive");

    if (withdrawnToday_ + m.amount() > dailyWithdrawCap_)
        throw InsufficientFunds("Vuot han muc rut trong ngay: han muc " +
                                Money{dailyWithdrawCap_, m.currency()}.toString());

    if (balance_.amount() < m.amount())
        throw InsufficientFunds("So du khong du: can " + m.toString() +
                                ", hien co " + balance_.toString());

    balance_        = Money{balance_.amount() - m.amount(), balance_.currency()};
    withdrawnToday_ += m.amount();
}

void SavingsAccount::applyInterest() {
    std::lock_guard<std::mutex> lock(mtx_);
    long double interest = balance_.amount() * interestRate_;
    balance_ = Money{balance_.amount() + interest, balance_.currency()};
}

void SavingsAccount::display(std::ostream& os) const {
    Account::display(os);
    os << "  Lai suat    : " << std::fixed << std::setprecision(2)
       << interestRate_ * 100.0L << "% / nam\n"
       << "  Han muc rut : " << Money{dailyWithdrawCap_, balance_.currency()}.toString()
       << " / ngay\n"
       << "  Da rut hom nay: " << Money{withdrawnToday_, balance_.currency()}.toString() << "\n";
}
