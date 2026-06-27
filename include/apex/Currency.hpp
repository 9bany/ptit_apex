#pragma once
#include <string>
#include <vector>

// Enum cac loai tien te duoc ho tro va cac ham tien ich chuyen doi, hien thi.
enum class Currency { VND, USD, EUR, JPY, GBP };

std::string currencyToString(Currency c);
Currency    currencyFromString(const std::string& s);
std::string currencySymbol(Currency c);
int         currencyDecimals(Currency c);
const std::vector<Currency>& allCurrencies();
