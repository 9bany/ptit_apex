#include "apex/Menu.hpp"
#include "apex/InputUtils.hpp"
#include "apex/SampleData.hpp"
#include "apex/DemoScenarios.hpp"
#include "apex/Errors.hpp"
#include "apex/IdGenerator.hpp"
#include "apex/Transaction.hpp"
#include "apex/Currency.hpp"
#include <iostream>
#include <string>

static void sep(char c = '=', int w = 60) {
    std::cout << std::string(w, c) << "\n";
}

Menu::Menu(Bank& bank) : bank_(bank) {}

void Menu::printHeader() const {
    sep();
    std::cout << "     HE THONG NGAN HANG DA TIEN TE  APEX\n";
    sep();
}

void Menu::printMainMenu() const {
    std::cout << "\n";
    std::cout << " [1]  Tao tai khoan moi\n";
    std::cout << " [2]  Nap tien\n";
    std::cout << " [3]  Rut tien\n";
    std::cout << " [4]  Chuyen khoan\n";
    std::cout << " [5]  Xem thong tin tai khoan\n";
    std::cout << " [6]  Lich su giao dich (theo tai khoan)\n";
    std::cout << " [7]  Nhat ky giao dich toan he thong\n";
    std::cout << " [8]  Cap nhat ty gia hoi doai\n";
    std::cout << " [9]  Xem ty gia hoi doai hien tai\n";
    std::cout << "[10]  Danh sach tat ca tai khoan\n";
    std::cout << "[11]  Tao du lieu mau\n";
    std::cout << "[12]  Demo kich ban loi\n";
    std::cout << " [0]  Thoat\n";
    sep('-');
}

void Menu::run() {
    printHeader();
    while (true) {
        printMainMenu();
        int choice = InputUtils::readInt("  Lua chon: ", 0, 12);
        std::cout << "\n";
        try {
            switch (choice) {
                case  0: std::cout << "  Tam biet!\n"; return;
                case  1: handleCreateAccount(); break;
                case  2: handleDeposit();       break;
                case  3: handleWithdraw();      break;
                case  4: handleTransfer();      break;
                case  5: handleViewAccount();   break;
                case  6: handleAccountHistory();break;
                case  7: handleGlobalLog();     break;
                case  8: handleSetRate();       break;
                case  9: handleViewRates();     break;
                case 10: handleListAccounts();  break;
                case 11: handleSampleData();    break;
                case 12: handleDemos();         break;
            }
        } catch (const ApexError& e) {
            std::cout << "  [LOI] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "  [LOI HE THONG] " << e.what() << "\n";
        }
        if (std::cin.good()) {
            std::cout << "\n  Nhan Enter de tiep tuc...";
            std::cin.ignore(1000, '\n');
        }
        if (!std::cin) break;
    }
}

void Menu::handleCreateAccount() {
    sep('-');
    std::cout << "  TAO TAI KHOAN MOI\n";
    sep('-');

    std::cout << "  Loai tai khoan:\n"
              << "  [1] Tiet kiem (Savings)\n"
              << "  [2] Thanh toan (Checking)\n";
    int type = InputUtils::readInt("  Lua chon: ", 1, 2);

    std::string owner  = InputUtils::readNonEmpty("  Ten chu tai khoan: ");
    Currency    curr   = InputUtils::readCurrency("  Don vi tien te");
    long double amount = InputUtils::readAmount("  So du ban dau: ");

    std::string id;
    if (type == 1) {
        long double rate  = 0.05L;
        long double limit = 50'000'000.0L;
        std::cout << "  Lai suat / nam (vi du 0.05 cho 5%) [mac dinh 0.05]: ";
        std::cin >> rate; InputUtils::clearLine();
        std::cout << "  Han muc rut / ngay [mac dinh 50,000,000]: ";
        std::cin >> limit; InputUtils::clearLine();
        id = bank_.createSavings(owner, Money{amount, curr}, rate, limit);
    } else {
        long double od = 5'000'000.0L;
        std::cout << "  Han muc thu chi [mac dinh 5,000,000]: ";
        std::cin >> od; InputUtils::clearLine();
        id = bank_.createChecking(owner, Money{amount, curr}, od);
    }

    std::cout << "  [+] Da tao tai khoan: " << id << " cho " << owner << "\n";
}

void Menu::handleDeposit() {
    sep('-');
    std::cout << "  NAP TIEN\n";
    sep('-');

    std::string id     = InputUtils::readAccountId("  Ma tai khoan: ");
    Currency    curr   = InputUtils::readCurrency("  Don vi tien te");
    long double amount = InputUtils::readAmount("  So tien nap: ");

    Money m{amount, curr};
    std::cout << "  [*] Goi: account + " << m << "  (operator+)\n";
    bank_.deposit(id, m);

    std::cout << "  [+] Nap tien thanh cong! So du moi: "
              << bank_.getAccount(id).balance() << "\n";
}

void Menu::handleWithdraw() {
    sep('-');
    std::cout << "  RUT TIEN\n";
    sep('-');

    std::string id     = InputUtils::readAccountId("  Ma tai khoan: ");
    Currency    curr   = InputUtils::readCurrency("  Don vi tien te");
    long double amount = InputUtils::readAmount("  So tien rut: ");

    Money m{amount, curr};
    std::cout << "  [*] Goi: account - " << m << "  (operator-)\n";
    bank_.withdraw(id, m);

    std::cout << "  [-] Rut tien thanh cong! So du moi: "
              << bank_.getAccount(id).balance() << "\n";
}

void Menu::handleTransfer() {
    sep('-');
    std::cout << "  CHUYEN KHOAN\n";
    sep('-');

    std::string srcId  = InputUtils::readAccountId("  Ma tai khoan nguon: ");
    std::string dstId  = InputUtils::readAccountId("  Ma tai khoan dich: ");
    Currency    curr   = InputUtils::readCurrency("  Don vi tien te");
    long double amount = InputUtils::readAmount("  So tien chuyen: ");

    bank_.transfer(srcId, dstId, Money{amount, curr});

    std::cout << "  [<->] Chuyen khoan thanh cong!\n"
              << "  So du tai khoan nguon: " << bank_.getAccount(srcId).balance() << "\n"
              << "  So du tai khoan dich : " << bank_.getAccount(dstId).balance() << "\n";
}

void Menu::handleViewAccount() {
    sep('-');
    std::cout << "  THONG TIN TAI KHOAN\n";
    sep('-');

    std::string id = InputUtils::readAccountId("  Ma tai khoan: ");
    bank_.getAccount(id).display(std::cout);
}

void Menu::handleAccountHistory() {
    sep('-');
    std::cout << "  LICH SU GIAO DICH THEO TAI KHOAN\n";
    sep('-');

    std::string id = InputUtils::readAccountId("  Ma tai khoan: ");
    bank_.log().printForAccount(id, std::cout);
}

void Menu::handleGlobalLog() {
    sep('-');
    std::cout << "  NHAT KY GIAO DICH TOAN HE THONG\n";
    sep('-');
    bank_.log().printAll(std::cout);
}

void Menu::handleSetRate() {
    sep('-');
    std::cout << "  CAP NHAT TY GIA HOI DOAI\n";
    sep('-');

    Currency    from = InputUtils::readCurrency("  Tien te goc ");
    Currency    to   = InputUtils::readCurrency("  Tien te dich");
    long double rate = InputUtils::readAmount("  Ty gia (phai > 0): ");

    bank_.converter().setRate(from, to, rate);

    bank_.log().record(Transaction{IdGenerator::nextTransactionId(),
                                   TransactionType::RateChange, "", "",
                                   Money{rate, to},
                                   "Ty gia " + currencyToString(from) + "->" +
                                   currencyToString(to) + " = " + std::to_string(rate)});

    std::cout << "  [+] Da cap nhat: 1 " << currencyToString(from)
              << " = " << rate << " " << currencyToString(to) << "\n";
}

void Menu::handleViewRates() {
    sep('-');
    std::cout << "  TY GIA HOI DOAI HIEN TAI\n";
    sep('-');
    bank_.converter().printRates(std::cout);
}

void Menu::handleListAccounts() {
    sep('-');
    std::cout << "  DANH SACH TAT CA TAI KHOAN\n";
    sep('-');
    bank_.displayAllAccounts(std::cout);
}

void Menu::handleSampleData() {
    sep('-');
    std::cout << "  TAO DU LIEU MAU\n";
    sep('-');
    SampleData::seed(bank_);
}

void Menu::handleDemos() {
    sep('-');
    std::cout << "  DEMO KICH BAN LOI\n";
    sep('-');
    std::cout << "  [1] Demo 'Chi tieu kep' (Double Spending)\n";
    std::cout << "  [2] Demo ty gia hoi doai khong hop le\n";
    std::cout << "  [0] Quay lai\n";

    int ch = InputUtils::readInt("  Lua chon: ", 0, 2);
    if (ch == 1) DemoScenarios::runDoubleSpend(bank_);
    if (ch == 2) DemoScenarios::runInvalidRate(bank_);
}
