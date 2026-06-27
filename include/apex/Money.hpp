#pragma once
#include "apex/Currency.hpp"
#include <ostream>
#include <string>

// Value object bieu dien so tien va tien te — bat bien sau khi tao.
class Money {
public:
    Money(long double amount, Currency currency)
        : amount_(amount), currency_(currency) {}

    long double amount()   const { return amount_; }
    Currency    currency() const { return currency_; }

    Money operator+(const Money& o) const;
    Money operator-(const Money& o) const;
    bool  operator>=(const Money& o) const;
    bool  operator<(const Money& o)  const;
    bool  operator<=(const Money& o) const;
    bool  operator==(const Money& o) const;

    std::string toString() const;

private:
    long double amount_;
    Currency    currency_;
};

std::ostream& operator<<(std::ostream& os, const Money& m);
