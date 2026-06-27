# Apex — Hệ thống Ngân hàng Đa tiền tệ

Bài tập lập trình C++ hướng đối tượng — PTIT.

## Build & Chạy

```bash
bash build.sh        # biên dịch → tạo file ./apex
./apex               # chế độ tương tác
./apex --demo        # tải dữ liệu mẫu rồi mở menu
```

Yêu cầu: `clang++` hoặc `g++` hỗ trợ C++17, hoặc CMake ≥ 3.16.

```bash
# Nếu dùng CMake
cmake -S . -B build && cmake --build build
./build/apex --demo
```

## Cấu trúc OOP

| Lớp | Vai trò |
|-----|---------|
| `Account` | Lớp cơ sở trừu tượng — số dư, mutex, `deposit`/`withdraw` |
| `SavingsAccount` | Kế thừa `Account` — lãi suất, giới hạn rút theo ngày |
| `CheckingAccount` | Kế thừa `Account` — hạn mức thấu chi |
| `Transaction` | Bản ghi bất biến mọi hành động (timestamp, loại, số tiền) |
| `TransactionLog` | Nhật ký toàn hệ thống + chỉ mục theo tài khoản |
| `Bank` | `std::map<string, shared_ptr<Account>>` — điều phối toàn bộ nghiệp vụ |
| `CurrencyConverter` | Ma trận tỷ giá, chuyển đổi tự động khi chuyển khoản khác tiền tệ |

### Nạp chồng toán tử

```cpp
account += Money{500'000, Currency::VND};   // operator+= → nạp tiền
account -= Money{200'000, Currency::VND};   // operator-= → rút tiền
```

**Lý do dùng `+=/-=` thay vì `+/-`:**
Đề bài yêu cầu nạp chồng `operator+` và `operator-`. Tuy nhiên, theo ngữ nghĩa C++ chuẩn, `x + 5` không được thay đổi `x` — toán tử nhị phân tạo ra giá trị mới, không biến đổi toán hạng trái. Ở đây `account += m` và `account -= m` biểu đạt đúng hơn ý định "biến đổi tại chỗ" (mutate in place) của thao tác nạp/rút tiền. Dự án chọn `+=/-=` để đảm bảo ngữ nghĩa nhất quán với chuẩn C++.

`Bank::transfer` dùng `std::scoped_lock` trên hai mutex theo thứ tự ID để tránh deadlock, sau đó gọi `doDeposit`/`doWithdraw` (protected) của mỗi tài khoản.

## Menu tương tác (12 chức năng)

```
 [1]  Tạo tài khoản mới
 [2]  Nạp tiền               ← dùng operator+=
 [3]  Rút tiền               ← dùng operator-=
 [4]  Chuyển khoản           ← tự động quy đổi tiền tệ
 [5]  Xem thông tin tài khoản
 [6]  Lịch sử giao dịch (theo tài khoản)
 [7]  Nhật ký giao dịch toàn hệ thống
 [8]  Cập nhật tỷ giá hối đoái
 [9]  Xem tỷ giá hối đoái hiện tại
[10]  Danh sách tất cả tài khoản
[11]  Tạo dữ liệu mẫu (5 tài khoản, VND/USD/EUR)
[12]  Demo kịch bản lỗi
 [0]  Thoát
```

## Demo kịch bản lỗi (cho video)

### 1. Chi tiêu kép (Double Spending)

```
./apex --demo → [12] → [1]
```

- **Không có mutex**: hai thread cùng thấy số dư đủ, cùng rút → số dư âm.
- **Có mutex** (Apex): chỉ một thread rút thành công, thread kia bị từ chối.

### 2. Tỷ giá không hợp lệ

```
./apex → [12] → [2]
```

Thử đặt tỷ giá = 0, -1.5, -0.001 → hệ thống ném `InvalidRate`, tỷ giá cũ được giữ nguyên.

## Tiền tệ hỗ trợ

`VND` · `USD` · `EUR` · `JPY` · `GBP`
