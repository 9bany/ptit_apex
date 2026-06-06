#pragma once
#include <atomic>
#include <string>

class IdGenerator {
public:
    static std::string nextAccountId();
    static std::string nextTransactionId();
    static void reset();

private:
    static std::atomic<int> accountCounter_;
    static std::atomic<int> transactionCounter_;
};
