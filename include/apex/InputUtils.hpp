#pragma once
#include "apex/Currency.hpp"
#include <string>

namespace InputUtils {
    void        clearLine();
    int         readInt(const std::string& prompt, int lo, int hi);
    long double readAmount(const std::string& prompt);
    std::string readNonEmpty(const std::string& prompt);
    Currency    readCurrency(const std::string& prompt);
    std::string readAccountId(const std::string& prompt);
}
