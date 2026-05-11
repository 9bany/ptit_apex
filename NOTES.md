# Apex — Ghi chú học C++

Dự án: Hệ thống ngân hàng đa tiền tệ, bài tập OOP PTIT.

---

## Cấu trúc tổng quan

```
main.cpp
  └─ Bank                    ← điều phối toàn bộ
       ├─ Account             ← lớp cơ sở trừu tượng
       │    ├─ SavingsAccount ← tiết kiệm (lãi suất, hạn mức rút/ngày)
       │    └─ CheckingAccount← thanh toán (cho phép thấu chi)
       ├─ Money               ← số tiền + đơn vị tiền tệ
       ├─ Currency            ← enum: VND, USD, EUR, JPY, GBP
       ├─ CurrencyConverter   ← bảng tỷ giá, tự đổi khi chuyển khoản
       ├─ Transaction         ← bản ghi 1 giao dịch (bất biến)
       └─ TransactionLog      ← nhật ký toàn hệ thống
```

---

## 1. enum class (Currency.cpp)

```cpp
// enum class an toàn hơn enum thường — không tự chuyển thành int
enum class Currency { VND, USD, EUR, JPY, GBP };

// Chuyển string → enum, ném exception nếu không hợp lệ
Currency currencyFromString(const std::string& s) {
    if (s == "VND") return Currency::VND;
    // ...
    throw InvalidInput("Unknown currency: " + s);
}
```

---

## 2. Operator Overloading (Money.cpp)

```cpp
// Nạp chồng toán tử để viết code tự nhiên hơn
Money Money::operator+(const Money& o) const {
    assertSameCurrency(*this, o);           // validate trước
    return Money{amount_ + o.amount_, currency_};
}

// So sánh số thực: KHÔNG dùng ==, phải dùng epsilon
bool Money::operator==(const Money& o) const {
    return currency_ == o.currency_ && std::fabsl(amount_ - o.amount_) < 1e-9L;
}
```

**Quy tắc:** Không bao giờ so sánh `float`/`double`/`long double` bằng `==`.

---

## 3. Kế thừa & Đa hình (Account / SavingsAccount / CheckingAccount)

### Member Initializer List

```cpp
// Cách khởi tạo đúng trong C++ — chạy trước thân constructor
Account::Account(const std::string& id, const std::string& owner, const Money& balance)
    : id_(id), owner_(owner), balance_(initialBalance) {}
//  ^^ Member Initializer List
```

### Hai tầng method — tránh deadlock nội bộ

```cpp
// PUBLIC — có lock (gọi từ bên ngoài)
void Account::deposit(const Money& m) {
    std::lock_guard<std::mutex> lock(mtx_);  // tự mở khóa khi ra scope (RAII)
    doDeposit(m);
}

// PROTECTED — không lock (gọi nội bộ khi mutex đã được giữ)
void Account::doDeposit(const Money& m) {
    balance_ = Money{balance_.amount() + m.amount(), balance_.currency()};
}
```

**Tại sao tách?** Nếu `deposit()` gọi `doDeposit()` mà cả hai đều lock cùng 1 mutex → deadlock ngay.

### Override hành vi rút tiền

```cpp
// SavingsAccount: thêm hạn mức rút theo ngày
void SavingsAccount::doWithdraw(const Money& m) {
    if (withdrawnToday_ + m.amount() > dailyWithdrawCap_)
        throw InsufficientFunds("Vuot han muc rut trong ngay");
    // ...
    withdrawnToday_ += m.amount();
}

// CheckingAccount: cho phép số dư âm đến hạn mức
void CheckingAccount::doWithdraw(const Money& m) {
    long double after = balance_.amount() - m.amount();
    if (after < -overdraftLimit_)
        throw OverdraftExceeded("Vuot han muc thu chi");
    balance_ = Money{after, balance_.currency()};
}
```

Cả hai override `doWithdraw()` — Bank gọi mà không cần biết loại tài khoản cụ thể. Đây là **đa hình (polymorphism)**.

---

## 4. Smart Pointer & STL (Bank.cpp)

```cpp
// shared_ptr: tự giải phóng bộ nhớ, không cần delete
std::map<std::string, std::shared_ptr<Account>> accounts_;

// Tạo đối tượng trên heap, bọc trong shared_ptr
accounts_[id] = std::make_shared<SavingsAccount>(id, owner, balance, rate, cap);

// Range-based for với structured binding (C++17)
for (auto& [id, acc] : accounts_) {
    acc->display(os);
}
```

---

## 5. Mutex & Thread Safety (Bank.cpp — transfer)

### Vấn đề: Deadlock khi chuyển khoản

Nếu thread A chuyển từ TK1→TK2, thread B chuyển TK2→TK1 cùng lúc:
- A giữ lock TK1, chờ TK2
- B giữ lock TK2, chờ TK1
- → **Deadlock** — cả hai chờ nhau mãi mãi

### Giải pháp: Luôn lock theo thứ tự cố định

```cpp
void Bank::transfer(const std::string& srcId, const std::string& dstId, ...) {
    Account* first  = srcPtr.get();
    Account* second = dstPtr.get();
    if (srcId > dstId) std::swap(first, second);  // ← sắp xếp theo ID

    std::scoped_lock lock(first->mutex(), second->mutex());  // lock cả 2 cùng lúc

    srcPtr->doWithdraw(amount);   // nếu throw → dstPtr chưa bị đổi → nhất quán
    dstPtr->doDeposit(converted);
}
```

**Nguyên tắc:** Khi cần lock nhiều mutex, luôn lock theo thứ tự nhất quán (ở đây: theo ID).

---

## 6. std::atomic & std::thread (IdGenerator + DemoScenarios)

```cpp
// atomic: đọc/ghi an toàn giữa nhiều thread, không cần mutex
std::atomic<int> IdGenerator::accountCounter_{1};

std::string IdGenerator::nextAccountId() {
    return format("ACC-", accountCounter_++);  // ++ trên atomic là thread-safe
}
```

### Demo race condition (Double Spending)

```cpp
// KHÔNG an toàn — hai thread cùng thấy số dư đủ, cùng rút
struct UnsafeBalance {
    long double balance = 1000.0;
    bool tryWithdraw(long double amount) {
        if (balance >= amount) {
            std::this_thread::sleep_for(50ms); // giả lập độ trễ mạng
            balance -= amount;  // ← HAI THREAD đến đây → số dư âm!
            return true;
        }
        return false;
    }
};

// AN TOÀN — Bank dùng mutex, chỉ 1 thread rút thành công
std::atomic<bool> go{false};
std::thread t1([&]{ while (!go) std::this_thread::yield(); bank.withdraw(id, m); });
std::thread t2([&]{ while (!go) std::this_thread::yield(); bank.withdraw(id, m); });
go = true;  // 2 thread bắt đầu cùng lúc
t1.join(); t2.join();
```

---

## 7. Exception Hierarchy

```
std::exception
  └─ ApexError          ← lỗi chung của hệ thống
       ├─ InsufficientFunds   ← số dư không đủ
       ├─ OverdraftExceeded   ← vượt hạn mức thấu chi
       ├─ CurrencyMismatch    ← sai đơn vị tiền tệ
       ├─ InvalidInput        ← input không hợp lệ
       ├─ InvalidRate         ← tỷ giá âm hoặc bằng 0
       └─ AccountNotFound     ← không tìm thấy tài khoản
```

```cpp
// Bắt theo thứ tự từ cụ thể → tổng quát
try {
    bank.transfer(src, dst, money);
} catch (const InsufficientFunds& e) {  // bắt cụ thể trước
    std::cout << "So du khong du: " << e.what();
} catch (const ApexError& e) {          // rồi mới tổng quát
    std::cout << "Loi he thong: " << e.what();
}
```

---

## 8. RAII — Quản lý tài nguyên tự động

Dự án dùng RAII ở nhiều nơi:

| Tài nguyên | RAII wrapper | Tự giải phóng khi |
|---|---|---|
| Mutex | `std::lock_guard` / `std::scoped_lock` | Ra khỏi scope `{}` |
| Heap memory | `std::shared_ptr` | Không còn ai dùng |
| Thread | `.join()` trong destructor (nên làm) | Object bị hủy |

```cpp
void Account::deposit(const Money& m) {
    std::lock_guard<std::mutex> lock(mtx_);
    doDeposit(m);
    // ← lock tự mở khi hàm kết thúc, dù có exception hay không
}
```

---

## 9. Namespace (InputUtils, SampleData, DemoScenarios)

```cpp
namespace InputUtils {
    int readInt(const std::string& prompt, int lo, int hi);
    long double readAmount(const std::string& prompt);
    // ...
}

// Gọi:
int choice = InputUtils::readInt("Lua chon: ", 0, 12);
```

Namespace nhóm các hàm tiện ích liên quan, tránh trùng tên.

---

## Thứ tự đọc code

1. `Currency.cpp` → `Money.cpp` — dữ liệu cơ bản
2. `Account.cpp` → `SavingsAccount.cpp` → `CheckingAccount.cpp` — OOP cốt lõi
3. `IdGenerator.cpp` — atomic
4. `Transaction.cpp` → `TransactionLog.cpp` — ghi nhật ký
5. `CurrencyConverter.cpp` — bảng tỷ giá
6. `Bank.cpp` — ghép tất cả, xem phần transfer (deadlock)
7. `DemoScenarios.cpp` — thread, race condition
8. `InputUtils.cpp` → `Menu.cpp` → `main.cpp` — UI

---

## Repo thiếu header files

Các file `include/apex/*.hpp` không có trong repo. Cần tạo mới để build được.
Chạy: `bash build.sh` hoặc `cmake -S . -B build && cmake --build build`
