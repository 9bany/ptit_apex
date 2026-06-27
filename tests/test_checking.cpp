#include "test_runner.hpp"
#include "apex/Bank.hpp"
#include "apex/Errors.hpp"

void suite_checking() {
    std::cout << "[Checking] Han muc thu chi\n";
    {
        Bank bank;
        // So du: 100,000 VND; han muc thu chi: 200,000 VND
        std::string id = bank.createChecking("Binh", Money{100'000, Currency::VND}, 200'000);

        EXPECT_NO_THROW("Rut vuot so du nhung trong han muc thu chi", {
            bank.withdraw(id, Money{250'000, Currency::VND}); // so du: -150,000 > -200,000: ok
        });
        EXPECT_THROW("Rut vuot ca han muc thu chi", OverdraftExceeded, {
            bank.withdraw(id, Money{100'000, Currency::VND}); // so du: -250,000 < -200,000: loi
        });
    }
}
