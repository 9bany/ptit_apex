#include "apex/Account.hpp"
#include "apex/Color.hpp"
#include "apex/Errors.hpp"
#include <iomanip>

Account::Account(const std::string& id, const std::string& owner, const Money& initialBalance)
    : id_(id), owner_(owner), balance_(initialBalance) {}

Money Account::balance() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return balance_;
}

void Account::doDeposit(const Money& m) {
    if (m.currency() != balance_.currency())
        throw CurrencyMismatch("Deposit currency does not match account currency");
    if (m.amount() <= 0.0L)
        throw InvalidInput("Deposit amount must be positive");
    balance_ = Money{balance_.amount() + m.amount(), balance_.currency()};
}

void Account::doWithdraw(const Money& m) {
    if (m.currency() != balance_.currency())
        throw CurrencyMismatch("Withdraw currency does not match account currency");
    if (m.amount() <= 0.0L)
        throw InvalidInput("Withdraw amount must be positive");
    checkWithdrawRules(m);
    balance_ = balance_ - m;
    onAfterWithdraw(m);
}

void Account::checkWithdrawRules(const Money& m) {
    if (balance_.amount() < m.amount())
        throw InsufficientFunds("So du khong du: can " + m.toString() +
                                ", hien co " + balance_.toString());
}

void Account::deposit(const Money& m) {
    std::lock_guard<std::mutex> lock(mtx_);
    doDeposit(m);
}

void Account::withdraw(const Money& m) {
    std::lock_guard<std::mutex> lock(mtx_);
    doWithdraw(m);
}

Account& Account::operator+=(const Money& m) {
    deposit(m);
    return *this;
}

Account& Account::operator-=(const Money& m) {
    withdraw(m);
    return *this;
}

void Account::display(std::ostream& os) const {
    std::lock_guard<std::mutex> lock(mtx_);
    os << Color::cyan << "  ID     : " << Color::reset << id_ << "\n"
       << Color::cyan << "  Loai   : " << Color::reset << type() << "\n"
       << Color::cyan << "  Chu TK : " << Color::reset << owner_ << "\n"
       << Color::cyan << "  So du  : " << Color::reset << balance_.toString() << "\n";
}
