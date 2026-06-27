#pragma once
#include "apex/Money.hpp"
#include <chrono>
#include <ostream>
#include <string>

// Ban ghi mot giao dich bat bien: loai, tai khoan, so tien, thoi diem.
enum class TransactionType { Deposit, Withdraw, Transfer, RateChange, AccountCreate };

std::string txTypeToString(TransactionType t);

class Transaction {
public:
    Transaction(const std::string& id, TransactionType type,
                const std::string& srcId, const std::string& dstId,
                const Money& money, const std::string& note = "");

    const std::string& id()    const { return id_; }
    TransactionType    type()  const { return type_; }
    const std::string& srcId() const { return srcId_; }
    const std::string& dstId() const { return dstId_; }
    const Money&       money() const { return money_; }
    const std::string& note()  const { return note_; }

    void print(std::ostream& os) const;

private:
    std::string      id_;
    TransactionType  type_;
    std::string      srcId_;
    std::string      dstId_;
    Money            money_;
    std::string      note_;
    std::chrono::system_clock::time_point timestamp_;
};
