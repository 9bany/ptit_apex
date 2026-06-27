#include "apex/Currency.hpp"
#include "apex/Errors.hpp"

std::string currencyToString(Currency c) {
    switch (c) {
        case Currency::VND: return "VND";
        case Currency::USD: return "USD";
        case Currency::EUR: return "EUR";
        case Currency::JPY: return "JPY";
        case Currency::GBP: return "GBP";
    }
    return "???";
}

Currency currencyFromString(const std::string& s) {
    if (s == "VND") return Currency::VND;
    if (s == "USD") return Currency::USD;
    if (s == "EUR") return Currency::EUR;
    if (s == "JPY") return Currency::JPY;
    if (s == "GBP") return Currency::GBP;
    throw InvalidInput("Unknown currency: " + s);
}

std::string currencySymbol(Currency c) {
    switch (c) {
        case Currency::VND: return "d";
        case Currency::USD: return "$";
        case Currency::EUR: return "EUR";
        case Currency::JPY: return "JPY";
        case Currency::GBP: return "GBP";
    }
    return "?";
}

int currencyDecimals(Currency c) {
    switch (c) {
        case Currency::VND: return 0;
        case Currency::JPY: return 0;
        default:            return 2;
    }
}

const std::vector<Currency>& allCurrencies() {
    static const std::vector<Currency> all = {
        Currency::VND, Currency::USD, Currency::EUR, Currency::JPY, Currency::GBP
    };
    return all;
}
