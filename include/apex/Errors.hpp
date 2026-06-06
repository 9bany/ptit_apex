#pragma once
#include <stdexcept>
#include <string>

struct ApexError : std::runtime_error {
    explicit ApexError(const std::string& msg) : std::runtime_error(msg) {}
};

struct InvalidInput : ApexError {
    explicit InvalidInput(const std::string& msg) : ApexError(msg) {}
};

struct CurrencyMismatch : ApexError {
    explicit CurrencyMismatch(const std::string& msg) : ApexError(msg) {}
};

struct InsufficientFunds : ApexError {
    explicit InsufficientFunds(const std::string& msg) : ApexError(msg) {}
};

struct OverdraftExceeded : ApexError {
    explicit OverdraftExceeded(const std::string& msg) : ApexError(msg) {}
};

struct InvalidRate : ApexError {
    explicit InvalidRate(const std::string& msg) : ApexError(msg) {}
};

struct AccountNotFound : ApexError {
    explicit AccountNotFound(const std::string& id)
        : ApexError("Khong tim thay tai khoan: " + id) {}
};
