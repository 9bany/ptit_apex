#pragma once
#include "apex/Currency.hpp"
#include "apex/Money.hpp"
#include <map>
#include <ostream>
#include <utility>

class CurrencyConverter {
public:
    CurrencyConverter();

    void        setRate(Currency from, Currency to, long double rate);
    long double getRate(Currency from, Currency to) const;
    Money       convert(const Money& m, Currency to) const;
    void        printRates(std::ostream& os) const;

private:
    static int key(Currency c) { return static_cast<int>(c); }

    std::map<std::pair<int,int>, long double> rates_;
};
