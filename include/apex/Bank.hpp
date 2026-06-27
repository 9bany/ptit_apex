#pragma once
#include "apex/Account.hpp"
#include "apex/SavingsAccount.hpp"
#include "apex/CheckingAccount.hpp"
#include "apex/CurrencyConverter.hpp"
#include "apex/TransactionLog.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>

class Bank {
public:
    Bank() = default;

    std::string createSavings(const std::string& owner, const Money& initialBalance,
                              long double interestRate, long double dailyWithdrawCap);
    std::string createChecking(const std::string& owner, const Money& initialBalance,
                               long double overdraftLimit);

    Account& getAccount(const std::string& id);

    void deposit(const std::string& id, const Money& m);
    void withdraw(const std::string& id, const Money& m);
    void transfer(const std::string& srcId, const std::string& dstId, const Money& amount);

    void displayAllAccounts(std::ostream& os) const;

    CurrencyConverter& converter() { return converter_; }
    TransactionLog&    log()       { return log_; }

private:
    std::map<std::string, std::shared_ptr<Account>> accounts_;
    mutable std::mutex                              mapMtx_;
    CurrencyConverter                               converter_;
    TransactionLog                                  log_;
};
