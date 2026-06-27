# Design Decisions — Apex

Ghi lai cac quyet dinh thiet ke quan trong: ly do chon huong nay, thay vi huong khac.
Doc cung voi ASSIGNMENT.md (yeu cau de bai) va CONVENTIONS.md (quy uoc code).

---

## 1. Moi tai khoan co dung 1 don vi tien te

**Quyet dinh:** `balance_` la `Money(amount, currency)` — tien te duoc gan co dinh khi tao tai khoan.

**Ly do:** Mo phong mo hinh ngan hang truyen thong (VCB, Vietinbank...):
- Tai khoan VND va tai khoan USD la hai tai khoan rieng biet
- Chuyen khoan khac tien te di qua `CurrencyConverter` — tuong duong bo phan doi ngoai te

**Thay the da xem xet:** Multi-currency wallet (kieu Wise/Revolut) — `balance_` la
`std::map<Currency, long double>`. Phuc tap hon dang ke, khong can thiet cho scope de bai.

**Anh huong:** Deposit/withdraw phai dung dung tien te cua tai khoan, neu khong nem `CurrencyMismatch`.

---

## 2. Dung `+=` / `-=` thay vi `+` / `-`

**Quyet dinh:** `account += money` (nap tien), `account -= money` (rut tien).

**Ly do:** Ngu nghia C++ chuan — `x + 5` khong duoc thay doi `x`.
Toan tu nhi phan tao gia tri moi, khong bien doi toan hang trai.
`account + money` doc hieu la "tao account moi" — sai voi nghiep vu nap/rut tien thuc su la bien doi tai cho.

**De bai yeu cau:** `operator+` va `operator-`. Chon `+=/-=` vi dung nghia hon,
ghi ro ly do trong README de giai trinh khi nop bai.

---

## 3. `doWithdraw` non-virtual — Template Method Pattern

**Quyet dinh:** `doWithdraw` la non-virtual, la bo khung co dinh.
Cac lop con override 2 hook: `checkWithdrawRules` va `onAfterWithdraw`.

**Ly do:** SavingsAccount va CheckingAccount deu copy 4 dong logic tu `Account::doWithdraw`
(kiem tra currency, kiem tra amount > 0, tru so du, goi hook). Vi pham DRY.

Template Method giai quyet: bo khung chay 1 lan trong lop cha,
lop con chi override phan khac biet (rule kiem tra so du, hau xu ly).

**Thay the da xem xet:** De moi lop con override toan bo `doWithdraw` — dan den copy-paste
4 dong validation o moi lop, kho bao tri.

---

## 4. `CheckingAccount::checkWithdrawRules` khong goi `Account::checkWithdrawRules`

**Quyet dinh:** CheckingAccount override `checkWithdrawRules` hoan toan, khong goi `Account::`.

**Ly do:** Rule cua CheckingAccount thay the hoan toan rule cua Account:
- Account: `balance >= amount` (khong cho so du am)
- CheckingAccount: `balance - amount >= -overdraftLimit` (cho so du am den han muc)

Hai rule mau thuan nhau — neu CheckingAccount goi `Account::checkWithdrawRules` truoc,
tai khoan co so du 100k va overdraft 200k se khong the rut 150k du hop le.

**So sanh:** SavingsAccount goi lai `Account::checkWithdrawRules` vi rule cua no
la BO SUNG (them check han muc ngay), khong phai thay the.

---

## 5. `friend class Bank` de truy cap `mtx_` truc tiep

**Quyet dinh:** Xoa `public mutex()` getter, Bank truy cap `mtx_` truc tiep qua friend.

**Ly do:** `mutex()` public cho phep bat ky code nao lock mutex cua Account tuy tien —
pha vo encapsulation. Chi Bank can truy cap mutex (de lock 2 tai khoan dong thoi trong transfer).
`friend class Bank` da co san, dung no dung muc dich hon la expose getter.

---

## 6. Huong phat trien tiep theo (neu mo rong)

| Tinh nang | Mo ta | Do phuc tap |
|---|---|---|
| Daily cap reset | Reset `withdrawnToday_` moi ngay qua `std::chrono` | Trung binh |
| Interest accrual | Tinh lai suat hang thang cho SavingsAccount | Trung binh |
| Multi-currency wallet | 1 tai khoan giu nhieu loai tien | Cao |
| Persistence | Luu/doc trang thai tu file (JSON/SQLite) | Cao |
| REST API | Thay Menu bang HTTP endpoints | Rat cao |
