# Coding Conventions — Apex

Tài liệu này áp dụng cho toàn bộ codebase Apex.
Đọc trước khi viết hoặc sửa bất kỳ file nào.

---

## 1. Đặt tên

| Thứ | Quy tắc | Ví dụ |
|---|---|---|
| Class | PascalCase | `Account`, `SavingsAccount` |
| Method / function | camelCase | `doWithdraw`, `getBalance` |
| Member variable | camelCase + trailing `_` | `balance_`, `owner_` |
| Local variable | camelCase | `afterWithdraw`, `rate` |
| Constant / enum value | PascalCase | `Currency::VND` |
| Namespace | PascalCase | `InputUtils`, `DemoScenarios` |
| File | PascalCase khớp tên class | `SavingsAccount.hpp` |

**Lý do trailing underscore:** phân biệt member với local/param, tránh nhầm trong constructor.

---

## 2. Tổ chức file

- `.hpp` — khai báo (interface): class, method signature, inline đơn giản
- `.cpp` — định nghĩa (implementation): thân hàm, logic
- Không đặt logic phức tạp trong `.hpp` trừ khi là `inline` thực sự cần thiết

### Include hygiene — quy tắc quan trọng

- Chỉ include những gì file đó **trực tiếp dùng**
- `<iostream>` — chỉ khi dùng `std::cout` / `std::cin` trực tiếp trong file đó
- `<ostream>` — khi chỉ nhận `std::ostream&` qua tham số
- `<utility>` — cho `std::swap`, `std::move`, `std::pair` (không phải `<algorithm>`)
- `<limits>` — giữ trong `.cpp`, không để trong `.hpp` nếu header không dùng
- Không include thứ đã được include gián tiếp qua header khác

---

## 3. Operator overloading

| Toán tử | Ngữ nghĩa bắt buộc | Ví dụ đúng |
|---|---|---|
| `a + b`, `a - b` | tạo giá trị mới, không đổi a/b | `Money::operator+` |
| `a += b`, `a -= b` | thay đổi a | `Account::operator+=` |

**Không viết `operator+` mà bên trong lại mutation** — phá vỡ principle of least surprise.

### Quyết định thiết kế: Account dùng `+=` / `-=` thay vì `+` / `-`

Đề bài yêu cầu "nạp chồng toán tử + và -" cho Account. Dự án chọn dùng `+=` / `-=` thay thế vì:

1. `acc + money` trông như tạo ra giá trị mới (giống `Money + Money`) — nhưng thực ra
   **thay đổi acc** — người đọc bị lừa.
2. `acc + money;` với kết quả bị bỏ đi trông như no-op — compiler nhiều lúc cảnh báo
   "expression result unused".
3. `acc += money` đọc là "acc cộng thêm money" — ký hiệu `=` báo trước có mutation,
   đúng với convention của C++ (int, vector, string đều dùng `+=` để mutate).

`+=` và `-=` thể hiện **cùng khái niệm** operator overloading mà đề bài muốn dạy,
nhưng đúng ngữ nghĩa hơn. Lý do lựa chọn này được ghi trong README.

---

## 4. Virtual và override

- `= 0` — chỉ dùng khi **không có giá trị mặc định hợp lý**
  - Ví dụ đúng: `type()` — mỗi loại tài khoản bắt buộc tự đặt tên
- `virtual f() {}` — dùng khi có default hợp lý (thường là "không làm gì")
  - Ví dụ đúng: `onAfterWithdraw()` — mặc định không cần làm gì thêm
- Luôn viết `override` ở class con — để compiler phát hiện nếu signature sai

---

## 5. Template Method Pattern

Khi class cha có bộ khung cố định, class con chỉ điền phần riêng:

```
doWithdraw()          ← non-virtual, không override được
  validate chung      ← viết 1 lần
  checkWithdrawRules()← virtual hook — con override phần riêng
  trừ tiền            ← viết 1 lần
  onAfterWithdraw()   ← virtual hook — con override nếu cần
```

---

## 6. Comment

- Viết comment **trong lúc viết code**, không để sau
- Comment giải thích **tại sao**, không giải thích **cái gì** (code tự nói được cái gì)
- Mỗi `.hpp`: một dòng mô tả class/namespace làm gì ở đầu file
- Hàm phức tạp: comment ngắn trước khai báo

```cpp
// Tiet kiem: co lai suat va han muc rut trong ngay.
class SavingsAccount : public Account { ... };

// Khoa ca hai tai khoan theo thu tu ID de tranh deadlock.
void transfer(const std::string& srcId, const std::string& dstId, const Money& amount);
```

---

## 7. Constructor

- Dùng member initializer list thay gán trong thân: `: id_(id), owner_(owner)`
- Constructor không làm gì thêm so với default → dùng `= default` trong header, xóa dòng trong `.cpp`

---

## 8. Git

- Commit sau mỗi phase hoàn thành (không commit code đang dang dở)
- Mỗi commit = một mốc hoạt động được
- Commit message: ngắn gọn, tiếng Anh, mô tả **việc gì** đã thay đổi
