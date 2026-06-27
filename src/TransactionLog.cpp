#include "apex/TransactionLog.hpp"

void TransactionLog::record(const Transaction& t) {
    std::lock_guard<std::mutex> lock(mtx_);
    size_t idx = all_.size();
    all_.push_back(t);

    if (!t.srcId().empty()) byAccount_[t.srcId()].push_back(idx);
    if (!t.dstId().empty() && t.dstId() != t.srcId())
        byAccount_[t.dstId()].push_back(idx);
}

std::vector<Transaction> TransactionLog::forAccount(const std::string& accountId) const {
    std::lock_guard<std::mutex> lock(mtx_);
    std::vector<Transaction> result;
    auto it = byAccount_.find(accountId);
    if (it != byAccount_.end())
        for (size_t idx : it->second) result.push_back(all_[idx]);
    return result;
}

void TransactionLog::printAll(std::ostream& os) const {
    std::lock_guard<std::mutex> lock(mtx_);
    if (all_.empty()) { os << "  (chua co giao dich nao)\n"; return; }
    for (auto& t : all_) t.print(os);
}

void TransactionLog::printForAccount(const std::string& accountId, std::ostream& os) const {
    auto txns = forAccount(accountId);
    if (txns.empty()) { os << "  (chua co giao dich nao cho tai khoan " << accountId << ")\n"; return; }
    for (auto& t : txns) t.print(os);
}
