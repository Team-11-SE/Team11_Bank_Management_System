# Sprint 1 test evidence

Test cases covered: **TC-ACC-01, TC-DP-01, TC-WD-01, TC-INQ-01, TC-INQ-02**
(from the Test Plan document). Command: `make test` (runs `tests/smoke_test.sh`).

## 1. Smoke test output

```text
gcc -Wall -Wextra -std=c11 -Iinclude -o bank src/main.c src/ui_menu.c src/accounts.c src/transactions.c src/filestore.c
bash tests/smoke_test.sh
== build ==
build ok

== run 1: create / deposit / refused withdraw / withdraw / balance / statement ==
PASS: account 1001 created
PASS: deposit 1500 -> balance 6500.00
PASS: withdraw of 7000 refused
PASS: withdraw 1000 -> balance 5500.00
PASS: balance inquiry
PASS: statement shows the withdrawal
PASS: statement shows the deposit
PASS: statement is newest first

== run 2: data still there after a restart ==
PASS: balance after restart

== data files ==
PASS: accounts.txt has the saved account
PASS: transactions.txt has the deposit
PASS: transactions.txt has the withdrawal

ALL CHECKS PASSED
```

## 2. Demo session (fresh start, same flow by hand)

```text
=====================================
  Team 11 Bank Management System
  Sprint 1 - basic account features
=====================================

Loaded 0 account(s) from data/accounts.txt.

1. Create account
2. Deposit
3. Withdraw
4. Balance inquiry
5. Mini statement (last 10 transactions)
0. Exit
Enter choice: 
Name: Invalid name. Use letters and spaces only, at least 2 characters.
Name: Opening balance: Account created. Your account number is 1001.

1. Create account
2. Deposit
3. Withdraw
4. Balance inquiry
5. Mini statement (last 10 transactions)
0. Exit
Enter choice: 
Account number: Deposit amount: Invalid amount. Enter a number greater than 0 with at most 2 decimals.
Deposit amount: Deposited 1500.00. New balance: 6500.00

1. Create account
2. Deposit
3. Withdraw
4. Balance inquiry
5. Mini statement (last 10 transactions)
0. Exit
Enter choice: 
Account number: Withdrawal amount: Insufficient balance. Available: 6500.00

1. Create account
2. Deposit
3. Withdraw
4. Balance inquiry
5. Mini statement (last 10 transactions)
0. Exit
Enter choice: 
Account number: Withdrawal amount: Withdrew 1000.00. New balance: 5500.00

1. Create account
2. Deposit
3. Withdraw
4. Balance inquiry
5. Mini statement (last 10 transactions)
0. Exit
Enter choice: 
Unknown option. Please choose between 0 and 5.

1. Create account
2. Deposit
3. Withdraw
4. Balance inquiry
5. Mini statement (last 10 transactions)
0. Exit
Enter choice: 
Account number: Balance for account 1001 (Asha): 5500.00

1. Create account
2. Deposit
3. Withdraw
4. Balance inquiry
5. Mini statement (last 10 transactions)
0. Exit
Enter choice: 
Account number: --- Last 2 transaction(s) for account 1001, newest first ---
2,1001,WITHDRAW,1000.00,5500.00,2026-10-13 11:10:21
1,1001,DEPOSIT,1500.00,6500.00,2026-10-13 11:10:21

1. Create account
2. Deposit
3. Withdraw
4. Balance inquiry
5. Mini statement (last 10 transactions)
0. Exit
Enter choice: 

Thank you for using Team 11 Bank. Bye!
```

## 3. Data files after the demo

```text
data/accounts.txt
1001,Asha,5500.00

data/transactions.txt
1,1001,DEPOSIT,1500.00,6500.00,<date-time>
2,1001,WITHDRAW,1000.00,5500.00,<date-time>
```

## 4. Damaged line check (BMS-F-017)

One garbage line was added as line 2 of `data/accounts.txt` and the program was started again:

```text
Warning: data/accounts.txt line 2 is damaged and was skipped.
Loaded 1 account(s) from data/accounts.txt.```

The program prints a warning with the line number, keeps the good account
and does not crash.

## 5. Notes

- Build is clean with `-Wall -Wextra` (`gcc`, tested on macOS and Linux).
- The mini statement prints the newest transaction first (BMS-F-023).
- Amounts are shown with 2 decimal places (BMS-F-022).
