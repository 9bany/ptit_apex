#include "apex/Bank.hpp"
#include "apex/IdGenerator.hpp"
#include "apex/Errors.hpp"
#include <iostream>
#include <algorithm>

std::string Bank::createSavings(const std::string& owner, const Money& initialBalance,
                                long double interestRate, long double dailyWithdrawCap) {
    std::string id = IdGenerator::nextAccountId();
    {
        std::lock_guard<std::mutex> lock(mapMtx_);
        accounts_[id] = std::make_shared<SavingsAccount>(
            id, owner, initialBalance, interestRate, dailyWithdrawCap);
    }
    log_.record(Transaction{IdGenerator::nextTransactionId(),
                            TransactionType::AccountCreate,
                            id, "", initialBalance,
                            "Tao tai khoan tiet kiem: " + owner});
    return id;
}

std::string Bank::createChecking(const std::string& owner, const Money& initialBalance,
                                 long double overdraftLimit) {
    std::string id = IdGenerator::nextAccountId();
    {
        std::lock_guard<std::mutex> lock(mapMtx_);
        accounts_[id] = std::make_shared<CheckingAccount>(id, owner, initialBalance, overdraftLimit);
    }
    log_.record(Transaction{IdGenerator::nextTransactionId(),
                            TransactionType::AccountCreate,
                            id, "", initialBalance,
                            "Tao tai khoan thanh toan: " + owner});
    return id;
}

Account& Bank::getAccount(const std::string& id) {
    std::lock_guard<std::mutex> lock(mapMtx_);
    auto it = accounts_.find(id);
    if (it == accounts_.end()) throw AccountNotFound(id);
    return *it->second;
}

void Bank::deposit(const std::string& id, const Money& m) {
    Account& acc = getAccount(id);
    acc += m;
    log_.record(Transaction{IdGenerator::nextTransactionId(),
                            TransactionType::Deposit, id, "", m});
}

void Bank::withdraw(const std::string& id, const Money& m) {
    Account& acc = getAccount(id);
    acc -= m;
    log_.record(Transaction{IdGenerator::nextTransactionId(),
                            TransactionType::Withdraw, id, "", m});
}

void Bank::transfer(const std::string& srcId, const std::string& dstId, const Money& amount) {
    // Fetch both accounts
    std::shared_ptr<Account> srcPtr, dstPtr;
    {
        std::lock_guard<std::mutex> lock(mapMtx_);
        auto itS = accounts_.find(srcId);
        auto itD = accounts_.find(dstId);
        if (itS == accounts_.end()) throw AccountNotFound(srcId);
        if (itD == accounts_.end()) throw AccountNotFound(dstId);
        srcPtr = itS->second;
        dstPtr = itD->second;
    }

    // Lock both in sorted ID order to prevent deadlock
    Account* first  = srcPtr.get();
    Account* second = dstPtr.get();
    if (srcId > dstId) std::swap(first, second);

    std::scoped_lock<std::mutex, std::mutex> lock(first->mtx_, second->mtx_);

    // Convert currency if accounts differ
    Money converted = converter_.convert(amount, dstPtr->balance_.currency());

    // Atomic debit + credit using protected methods
    srcPtr->doWithdraw(amount);      // may throw InsufficientFunds — no partial mutation
    dstPtr->doDeposit(converted);

    log_.record(Transaction{IdGenerator::nextTransactionId(),
                            TransactionType::Transfer,
                            srcId, dstId, amount,
                            "Chuyen khoan: " + amount.toString() +
                            " -> " + converted.toString()});
}

void Bank::displayAllAccounts(std::ostream& os) const {
    std::lock_guard<std::mutex> lock(mapMtx_);
    if (accounts_.empty()) {
        os << "  (chua co tai khoan nao)\n";
        return;
    }
    for (auto& [id, acc] : accounts_) {
        os << "  " << std::string(46, '-') << "\n";
        acc->display(os);
    }
    os << "  " << std::string(46, '-') << "\n";
}
