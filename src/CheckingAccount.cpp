#include "apex/CheckingAccount.hpp"
#include "apex/Errors.hpp"
#include <iostream>
#include <iomanip>

CheckingAccount::CheckingAccount(const std::string& id, const std::string& owner,
                                 const Money& initialBalance, long double overdraftLimit)
    : Account(id, owner, initialBalance), overdraftLimit_(overdraftLimit) {}

void CheckingAccount::doWithdraw(const Money& m) {
    if (m.currency() != balance_.currency())
        throw CurrencyMismatch("Currency mismatch");
    if (m.amount() <= 0.0L)
        throw InvalidInput("Withdraw amount must be positive");

    long double afterWithdraw = balance_.amount() - m.amount();
    if (afterWithdraw < -overdraftLimit_)
        throw OverdraftExceeded("Vuot han muc thu chi: can " + m.toString() +
                                ", so du hien tai " + balance_.toString() +
                                ", han muc thu chi " +
                                Money{overdraftLimit_, m.currency()}.toString());

    balance_ = Money{afterWithdraw, balance_.currency()};
}

void CheckingAccount::display(std::ostream& os) const {
    Account::display(os);
    os << "  Han muc thu chi: " << Money{overdraftLimit_, balance_.currency()}.toString() << "\n";
}
