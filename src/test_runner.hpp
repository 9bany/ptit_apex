#pragma once
#include <iostream>
#include <string>

// Bien dem ket qua dung chung — dinh nghia trong test_bank.cpp.
extern int g_passed;
extern int g_failed;

inline void ok(const std::string& name) {
    std::cout << "  [PASS] " << name << "\n";
    ++g_passed;
}

inline void fail(const std::string& name, const std::string& reason) {
    std::cout << "  [FAIL] " << name << " -- " << reason << "\n";
    ++g_failed;
}

#define EXPECT_THROW(name, T, block)          \
    try {                                     \
        block;                                \
        fail(name, "khong nem exception");    \
    } catch (const T&) {                      \
        ok(name);                             \
    } catch (...) {                           \
        fail(name, "sai loai exception");     \
    }

#define EXPECT_NO_THROW(name, block)                              \
    try {                                                         \
        block;                                                    \
        ok(name);                                                 \
    } catch (const std::exception& e) {                           \
        fail(name, std::string("nem ngoai le: ") + e.what());    \
    }
