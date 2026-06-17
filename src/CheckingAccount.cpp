#include "apex/CheckingAccount.hpp"
#include "apex/Errors.hpp"
#include <iomanip>

CheckingAccount::CheckingAccount(const std::string& id, const std::string& owner,
                                 const Money& initialBalance, long double overdraftLimit)
    : Account(id, owner, initialBalance), overdraftLimit_(overdraftLimit) {}

void CheckingAccount::checkWithdrawRules(const Money& m) {
    if (balance_.amount() - m.amount() < -overdraftLimit_)
        throw OverdraftExceeded("Vuot han muc thu chi: can " + m.toString() +
                                ", so du hien tai " + balance_.toString() +
                                ", han muc thu chi " +
                                Money{overdraftLimit_, m.currency()}.toString());
}

void CheckingAccount::display(std::ostream& os) const {
    Account::display(os);
    os << "  Han muc thu chi: " << Money{overdraftLimit_, balance_.currency()}.toString() << "\n";
}
