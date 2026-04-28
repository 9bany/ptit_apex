#include "apex/Transaction.hpp"
#include <iomanip>
#include <ctime>
#include <sstream>

std::string txTypeToString(TransactionType t) {
    switch (t) {
        case TransactionType::Deposit:       return "NAP TIEN";
        case TransactionType::Withdraw:      return "RUT TIEN";
        case TransactionType::Transfer:      return "CHUYEN KHOAN";
        case TransactionType::RateChange:    return "CAP NHAT TY GIA";
        case TransactionType::AccountCreate: return "TAO TAI KHOAN";
    }
    return "???";
}

Transaction::Transaction(const std::string& id, TransactionType type,
                         const std::string& srcId, const std::string& dstId,
                         const Money& money, const std::string& note)
    : id_(id), type_(type), srcId_(srcId), dstId_(dstId),
      money_(money), note_(note),
      timestamp_(std::chrono::system_clock::now()) {}

void Transaction::print(std::ostream& os) const {
    auto t  = std::chrono::system_clock::to_time_t(timestamp_);
    auto tm = *std::localtime(&t);

    std::ostringstream ts;
    ts << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    os << "  [" << id_ << "] " << ts.str()
       << " | " << std::left << std::setw(16) << txTypeToString(type_)
       << " | " << money_.toString();

    if (!srcId_.empty()) os << " | Tu: " << srcId_;
    if (!dstId_.empty()) os << " -> " << dstId_;
    if (!note_.empty())  os << " | " << note_;
    os << "\n";
}
