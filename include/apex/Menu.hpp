#pragma once
#include "apex/Bank.hpp"

// Giao dien dong lenh tuong tac voi nguoi dung — dieu phoi cac lenh goi Bank.
class Menu {
public:
    explicit Menu(Bank& bank);
    void run();

private:
    Bank& bank_;

    void printHeader()    const;
    void printMainMenu()  const;

    void handleCreateAccount();
    void handleDeposit();
    void handleWithdraw();
    void handleTransfer();
    void handleViewAccount();
    void handleAccountHistory();
    void handleGlobalLog();
    void handleSetRate();
    void handleViewRates();
    void handleListAccounts();
    void handleSampleData();
    void handleDemos();
};
