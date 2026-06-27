#include "apex/DemoScenarios.hpp"
#include "apex/Color.hpp"
#include "apex/Errors.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include <iomanip>

namespace DemoScenarios {

// ─────────────────────────────────────────────────────────────
//  DEMO 1: Chi tieu kep (Double Spending)
// ─────────────────────────────────────────────────────────────

// Simulate a race-prone bank without a mutex: TOCTOU vulnerability
struct UnsafeBalance {
    long double balance;
    std::string currency;

    bool tryWithdraw(long double amount, const std::string& threadName) {
        std::cout << "    [" << threadName << "] Kiem tra so du: "
                  << std::fixed << std::setprecision(2) << balance
                  << " " << currency << "\n";

        if (balance >= amount) {
            // Simulate processing delay (ATM network round-trip)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            balance -= amount;
            std::cout << "    [" << threadName << "] >>> RUT THANH CONG! So du con lai: "
                      << balance << " " << currency << "\n";
            return true;
        }
        std::cout << "    [" << threadName << "] XXX Bi tu choi: so du khong du\n";
        return false;
    }
};

static void demoWithoutLock() {
    std::cout << Color::yellow
              << "\n  ╔══════════════════════════════════════════════════╗\n"
              << "  ║  KHONG CO MUTEX - Hai dau cuoi cung luc rut     ║\n"
              << "  ╚══════════════════════════════════════════════════╝\n"
              << Color::reset;

    UnsafeBalance acct;
    acct.balance  = 1000.0;
    acct.currency = "USD";
    std::cout << "  So du ban dau: 1,000.00 USD\n";
    std::cout << "  Hai thiet bi dau cuoi cung thu rut 1,000.00 USD...\n";
    std::cout << Color::dim
              << "  (Output phia duoi co the bi chen nhau — day chinh la bieu hien race condition)\n"
              << Color::reset << "\n";

    // Use a flag to sync both threads to start simultaneously
    std::atomic<bool> go{false};

    auto tryThread = [&](const std::string& name) {
        while (!go.load()) std::this_thread::yield();
        acct.tryWithdraw(1000.0, name);
    };

    std::thread t1(tryThread, "Terminal-1");
    std::thread t2(tryThread, "Terminal-2");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    go = true;
    t1.join();
    t2.join();

    std::cout << "\n  >>> KET QUA: So du cuoi cung = "
              << std::fixed << std::setprecision(2) << acct.balance << " USD";
    if (acct.balance < 0.0) {
        std::cout << Color::red << "  <-- AM! CHI TIEU KEP THANH CONG (loi bao mat!)" << Color::reset;
    }
    std::cout << "\n";
}

static void demoWithLock(Bank& bank) {
    std::cout << Color::green
              << "\n  ╔══════════════════════════════════════════════════╗\n"
              << "  ║  CO MUTEX - He thong Apex khoa tai khoan         ║\n"
              << "  ╚══════════════════════════════════════════════════╝\n"
              << Color::reset;

    // Create a temporary demo account with zero overdraft so the 2nd withdrawal fails
    std::string id = bank.createChecking("Demo-DoubleSpend", Money{1000.0L, Currency::USD}, 0.0L);
    std::cout << "  Tao tai khoan tam: " << id << " | So du: 1,000.00 USD\n";
    std::cout << "  Hai thiet bi dau cuoi cung thu rut 1,000.00 USD...\n\n";

    std::atomic<bool>  go{false};
    std::atomic<int>   successCount{0};
    std::mutex         printMtx;

    auto tryWithdraw = [&](const std::string& name) {
        while (!go.load()) std::this_thread::yield();
        try {
            {
                std::lock_guard<std::mutex> pl(printMtx);
                std::cout << "    [" << name << "] Dang thu rut 1,000.00 USD...\n";
            }
            bank.withdraw(id, Money{1000.0L, Currency::USD});
            ++successCount;
            std::lock_guard<std::mutex> pl(printMtx);
            std::cout << Color::green << "    [" << name << "] >>> RUT THANH CONG!" << Color::reset << "\n";
        } catch (const ApexError& e) {
            std::lock_guard<std::mutex> pl(printMtx);
            std::cout << Color::red << "    [" << name << "] XXX Bi tu choi: " << Color::reset << e.what() << "\n";
        }
    };

    std::thread t1(tryWithdraw, "Terminal-1");
    std::thread t2(tryWithdraw, "Terminal-2");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    go = true;
    t1.join();
    t2.join();

    Money finalBal = bank.getAccount(id).balance();
    std::cout << "\n  >>> KET QUA: So du cuoi cung = " << finalBal
              << " | So lan rut thanh cong: " << successCount.load() << "/2\n";
    if (successCount.load() <= 1) {
        std::cout << Color::green << "  [OK] Mutex bao ve thanh cong! Chi 1 giao dich duoc chap thuan." << Color::reset << "\n";
    }
}

void runDoubleSpend(Bank& bank) {
    std::cout << "\n============================================================\n";
    std::cout << "  DEMO: CHI TIEU KEP (Double Spending)\n";
    std::cout << "============================================================\n";

    demoWithoutLock();
    std::cout << "\n  Nhan Enter de xem phien ban CO mutex...\n";
    std::cin.ignore();
    demoWithLock(bank);

    std::cout << "\n  [KET LUAN] Mutex dam bao chi mot giao dich duoc xu ly\n"
              << "  tai mot thoi diem, ngan chan chi tieu kep.\n";
}

// ─────────────────────────────────────────────────────────────
//  DEMO 2: Ty gia khong hop le
// ─────────────────────────────────────────────────────────────

void runInvalidRate(Bank& bank) {
    std::cout << "\n============================================================\n";
    std::cout << "  DEMO: TY GIA HOI DOAI KHONG HOP LE\n";
    std::cout << "============================================================\n";

    long double badRates[] = {0.0L, -1.5L, -0.001L};

    for (long double rate : badRates) {
        std::cout << "\n  Thu dat ty gia USD -> VND = " << rate << ":\n";
        try {
            bank.converter().setRate(Currency::USD, Currency::VND, rate);
            std::cout << Color::red << "  [!] Khong nen den day — ty gia da duoc dat sai!" << Color::reset << "\n";
        } catch (const InvalidRate& e) {
            std::cout << Color::green << "  [OK] He thong tu choi: " << Color::reset << e.what() << "\n";
        }
    }

    // Verify old rate is still intact
    std::cout << "\n  Kiem tra ty gia sau cac lan thu sai:\n";
    try {
        long double cur = bank.converter().getRate(Currency::USD, Currency::VND);
        std::cout << "  Ty gia USD->VND hien tai: " << std::fixed
                  << std::setprecision(2) << cur << " (van giu nguyen)\n";
    } catch (const ApexError& e) {
        std::cout << "  Loi: " << e.what() << "\n";
    }

    std::cout << Color::green
              << "\n  [KET LUAN] He thong chi chap nhan ty gia duong.\n"
              << "  Ty gia khong hop le bi tu choi va ty gia cu van duoc giu nguyen.\n"
              << Color::reset;
}

} // namespace DemoScenarios
