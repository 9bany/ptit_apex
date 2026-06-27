#pragma once
#include "apex/Account.hpp"

// Tai khoan thanh toan: cho phep so du am den han muc thu chi.
class CheckingAccount : public Account {
public:
    CheckingAccount(const std::string& id, const std::string& owner,
                    const Money& initialBalance, long double overdraftLimit);

    std::string type() const override { return "Thanh toan"; }
    void display(std::ostream& os) const override;

protected:
    void checkWithdrawRules(const Money& m) override;
    // onAfterWithdraw khong can — dung mac dinh rong tu Account

private:
    long double overdraftLimit_;
};
