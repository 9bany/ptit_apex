#include "apex/InputUtils.hpp"
#include "apex/Errors.hpp"
#include <iostream>
#include <limits>
#include <sstream>

namespace InputUtils {

void clearLine() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readInt(const std::string& prompt, int lo, int hi) {
    while (true) {
        std::cout << prompt;
        int v;
        if (std::cin >> v) {
            clearLine();
            if (v >= lo && v <= hi) return v;
            std::cout << "  [!] Vui long nhap so tu " << lo << " den " << hi << ".\n";
        } else if (std::cin.eof()) {
            return lo; // EOF — return minimum (triggers exit in menu)
        } else {
            clearLine();
            std::cout << "  [!] Gia tri khong hop le. Thu lai.\n";
        }
    }
}

long double readAmount(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        long double v;
        if (std::cin >> v) {
            clearLine();
            if (v > 0.0L) return v;
            std::cout << "  [!] So tien phai lon hon 0.\n";
        } else {
            clearLine();
            std::cout << "  [!] Gia tri khong hop le. Thu lai.\n";
        }
    }
}

std::string readNonEmpty(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        if (!s.empty()) return s;
        std::cout << "  [!] Khong duoc de trong.\n";
    }
}

Currency readCurrency(const std::string& prompt) {
    while (true) {
        std::cout << prompt << " [VND/USD/EUR/JPY/GBP]: ";
        std::string s;
        std::cin >> s;
        clearLine();
        try { return currencyFromString(s); }
        catch (...) {
            std::cout << "  [!] Loai tien te khong hop le. Thu lai.\n";
        }
    }
}

std::string readAccountId(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::cin >> s;
        clearLine();
        if (!s.empty()) return s;
        std::cout << "  [!] ID khong duoc de trong.\n";
    }
}

} // namespace InputUtils
