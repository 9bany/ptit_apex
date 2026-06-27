#pragma once
#include "apex/Transaction.hpp"
#include <map>
#include <mutex>
#include <ostream>
#include <string>
#include <vector>

// Lich su giao dich toan he thong — thread-safe, tra cuu theo tai khoan.
class TransactionLog {
public:
    void record(const Transaction& t);
    std::vector<Transaction> forAccount(const std::string& accountId) const;
    void printAll(std::ostream& os) const;
    void printForAccount(const std::string& accountId, std::ostream& os) const;

private:
    std::vector<Transaction>                all_;
    std::map<std::string, std::vector<size_t>> byAccount_;
    mutable std::mutex                      mtx_;
};
