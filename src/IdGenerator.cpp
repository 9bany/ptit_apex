#include "apex/IdGenerator.hpp"
#include <sstream>
#include <iomanip>

std::atomic<int> IdGenerator::accountCounter_{1};
std::atomic<int> IdGenerator::transactionCounter_{1};

static std::string format(const std::string& prefix, int n) {
    std::ostringstream oss;
    oss << prefix << std::setw(4) << std::setfill('0') << n;
    return oss.str();
}

std::string IdGenerator::nextAccountId() {
    return format("ACC-", accountCounter_++);
}

std::string IdGenerator::nextTransactionId() {
    return format("TXN-", transactionCounter_++);
}

void IdGenerator::reset() {
    accountCounter_     = 1;
    transactionCounter_ = 1;
}
