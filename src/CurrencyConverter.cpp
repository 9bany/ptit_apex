#include "apex/CurrencyConverter.hpp"
#include "apex/Errors.hpp"
#include <iomanip>
#include <iostream>

CurrencyConverter::CurrencyConverter() {
    // Default rates (VND base: 1 unit = X VND)
    auto set = [&](Currency f, Currency t, long double r) {
        rates_[{key(f), key(t)}] = r;
        if (r > 0.0L) rates_[{key(t), key(f)}] = 1.0L / r;
    };

    set(Currency::USD, Currency::VND, 25'000.0L);
    set(Currency::EUR, Currency::VND, 27'000.0L);
    set(Currency::JPY, Currency::VND, 165.0L);
    set(Currency::GBP, Currency::VND, 31'500.0L);
    set(Currency::EUR, Currency::USD, 1.08L);
    set(Currency::GBP, Currency::USD, 1.26L);
    set(Currency::JPY, Currency::USD, 0.0066L);
    set(Currency::GBP, Currency::EUR, 1.167L);
}

void CurrencyConverter::setRate(Currency from, Currency to, long double rate) {
    if (rate <= 0.0L)
        throw InvalidRate("Ty gia phai duong, nhung nhan duoc: " +
                         std::to_string(static_cast<double>(rate)));
    rates_[{key(from), key(to)}] = rate;
    rates_[{key(to), key(from)}] = 1.0L / rate;
}

long double CurrencyConverter::getRate(Currency from, Currency to) const {
    if (from == to) return 1.0L;
    auto it = rates_.find({key(from), key(to)});
    if (it != rates_.end()) return it->second;

    // Triangulate via VND
    auto itA = rates_.find({key(from), key(Currency::VND)});
    auto itB = rates_.find({key(Currency::VND), key(to)});
    if (itA != rates_.end() && itB != rates_.end())
        return itA->second * itB->second;

    throw InvalidRate("Khong co ty gia cho " +
                     currencyToString(from) + " -> " + currencyToString(to));
}

Money CurrencyConverter::convert(const Money& m, Currency to) const {
    if (m.currency() == to) return m;
    long double rate = getRate(m.currency(), to);
    return Money{m.amount() * rate, to};
}

void CurrencyConverter::printRates(std::ostream& os) const {
    os << "\n  TY GIA HOI DOAI HIEN TAI:\n";
    os << "  " << std::string(50, '-') << "\n";
    for (auto& [pair, rate] : rates_) {
        Currency f = static_cast<Currency>(pair.first);
        Currency t = static_cast<Currency>(pair.second);
        os << "  1 " << std::left << std::setw(4) << currencyToString(f)
           << " = " << std::fixed << std::setprecision(4) << rate
           << " " << currencyToString(t) << "\n";
    }
}
