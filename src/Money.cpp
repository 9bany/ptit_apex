#include "apex/Money.hpp"
#include "apex/Errors.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

static void assertSameCurrency(const Money& a, const Money& b) {
    if (a.currency() != b.currency())
        throw CurrencyMismatch("Khong the tinh toan giua " + currencyToString(a.currency()) +
                               " va " + currencyToString(b.currency()));
}

Money Money::operator+(const Money& o) const {
    assertSameCurrency(*this, o);
    return Money{amount_ + o.amount_, currency_};
}

Money Money::operator-(const Money& o) const {
    assertSameCurrency(*this, o);
    return Money{amount_ - o.amount_, currency_};
}

bool Money::operator>=(const Money& o) const {
    assertSameCurrency(*this, o);
    return amount_ >= o.amount_;
}

bool Money::operator<(const Money& o) const {
    assertSameCurrency(*this, o);
    return amount_ < o.amount_;
}

bool Money::operator<=(const Money& o) const {
    assertSameCurrency(*this, o);
    return amount_ <= o.amount_;
}

bool Money::operator==(const Money& o) const {
    return currency_ == o.currency_ && std::fabs(amount_ - o.amount_) < 1e-9L;
}

static std::string formatNumber(long double value, int decimals) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << value;
    std::string s = oss.str();

    // Insert thousands separators
    size_t dotPos = s.find('.');
    size_t intEnd = (dotPos == std::string::npos) ? s.size() : dotPos;
    int insertPos = static_cast<int>(intEnd) - 3;
    while (insertPos > 0) {
        s.insert(static_cast<size_t>(insertPos), ",");
        insertPos -= 3;
    }
    return s;
}

std::string Money::toString() const {
    int dec = currencyDecimals(currency_);
    return formatNumber(amount_, dec) + " " + currencyToString(currency_);
}

std::ostream& operator<<(std::ostream& os, const Money& m) {
    return os << m.toString();
}
