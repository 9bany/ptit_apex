#pragma once
#include "apex/Money.hpp"
#include <mutex>
#include <ostream>
#include <string>

class Bank;

class Account {
public:
    Account(const std::string& id, const std::string& owner, const Money& initialBalance);
    virtual ~Account() = default;

    std::string  id()    const { return id_; }
    std::string  owner() const { return owner_; }
    Money        balance() const;
    std::mutex&  mutex() { return mtx_; }

    void deposit(const Money& m);
    void withdraw(const Money& m);

    Account& operator+=(const Money& m);
    Account& operator-=(const Money& m);

    virtual std::string type()                    const = 0;
    virtual void        display(std::ostream& os) const;

protected:
    virtual void doDeposit(const Money& m);
    void         doWithdraw(const Money& m);          // template — khong override duoc

    virtual void checkWithdrawRules(const Money& m);  // hook: mac dinh kiem tra so du
    virtual void onAfterWithdraw(const Money& m) {}   // hook: mac dinh khong lam gi

    std::string         id_;
    std::string         owner_;
    Money               balance_;
    mutable std::mutex  mtx_;

    friend class Bank;
};

Account& operator+(Account& dst, const Money& m);
Account& operator-(Account& src, const Money& m);
