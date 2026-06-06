#pragma once
#include "apex/Account.hpp"

class CheckingAccount : public Account {
public:
    CheckingAccount(const std::string& id, const std::string& owner,
                    const Money& initialBalance, long double overdraftLimit);

    std::string type() const override { return "Thanh toan"; }
    void display(std::ostream& os) const override;

protected:
    void doWithdraw(const Money& m) override;

private:
    long double overdraftLimit_;
};
