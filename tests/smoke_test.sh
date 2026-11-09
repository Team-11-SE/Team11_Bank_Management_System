
#!/usr/bin/env bash
# Smoke test for the Team 11 Bank program (Sprint 2).
# Run from the repo root with: make test (or: bash tests/smoke_test.sh)
set -u

cd "$(dirname "$0")/.."

fail=0

check() {                       # $1 = pattern to find, $2 = description
    if printf '%s' "$OUT" | grep -q "$1"; then
        echo "PASS: $2"
    else
        echo "FAIL: $2"
        fail=1
    fi
}

file_check() {                  # $1 = pattern, $2 = file, $3 = description
    if grep -q "$1" "$2" 2>/dev/null; then
        echo "PASS: $3"
    else
        echo "FAIL: $3"
        fail=1
    fi
}

secret_check() {                # $1 = secret, $2 = description
    if grep -rq -- "$1" data/ 2>/dev/null; then
        echo "FAIL: $2"
        fail=1
    else
        echo "PASS: $2"
    fi
}

rm -f data/accounts.txt data/transactions.txt
rm -rf data/backup

echo "== build =="
if ! make >/dev/null; then
    echo "FAIL: build"
    exit 1
fi
echo "build ok"

echo
echo "== create an account with a 4-digit PIN =="
OUT=$(printf '2\nAsha\n5000\n4829\n4829\n0\n' | ./bank)
check "Your account number is 1001" "account 1001 created"
if grep -Eq '^1001,Asha,[0-9a-f]{16},5000\.00,ACTIVE,0$' data/accounts.txt 2>/dev/null; then
    echo "PASS: account saved with a PIN hash"
else
    echo "FAIL: account saved with a PIN hash"
    fail=1
fi
secret_check "4829" "PIN is not stored in plain text"

echo
echo "== a wrong PIN is refused =="
OUT=$(printf '1\n1001\n0000\n0\n' | ./bank)
check "Wrong PIN" "wrong PIN refused"

echo
echo "== 3 wrong PINs lock an account (S2-02) =="
OUT=$(printf '2\nBob\n2000\n3456\n3456\n1\n1002\n0000\n1\n1002\n0000\n1\n1002\n0000\n1\n1002\n3456\n0\n' | ./bank)
check "Account is LOCKED" "3 wrong PINs lock the account"
check "Account is LOCKED. Contact the bank admin." "correct PIN refused while locked"
if grep -Eq '^1002,Bob,[0-9a-f]{16},2000\.00,LOCKED,3$' data/accounts.txt 2>/dev/null; then
    echo "PASS: the LOCKED status is saved"
else
    echo "FAIL: the LOCKED status is saved"
    fail=1
fi

echo
echo "== login and money operations =="
OUT=$(printf '1\n1001\n4829\n1\n1500\n2\n1000\n3\n4\n0\n0\n' | ./bank)
check "Login successful" "login with the correct PIN"
check "Deposited 1500.00. New balance: 6500.00" "deposit 1500 -> balance 6500.00"
check "Withdrew 1000.00. New balance: 5500.00" "withdraw 1000 -> balance 5500.00"
check "Balance for account 1001 (Asha): 5500.00" "balance inquiry"
check "WITHDRAW,1000.00" "statement shows the withdrawal"
check "DEPOSIT,1500.00" "statement shows the deposit"

# newest first check (BMS-F-023)
first=$(printf '%s\n' "$OUT" | grep -n "WITHDRAW,1000.00" | head -1 | cut -d: -f1)
second=$(printf '%s\n' "$OUT" | grep -n "DEPOSIT,1500.00" | head -1 | cut -d: -f1)
if [ -n "$first" ] && [ -n "$second" ] && [ "$first" -lt "$second" ]; then
    echo "PASS: statement is newest first"
else
    echo "FAIL: statement is newest first"
    fail=1
fi

echo
echo "== data still there after a restart =="
echo
echo "== minimum balance and daily withdrawal limit (S2-04) =="
OUT=$(printf '1\n1001\n4829\n2\n7000\n0\n0\n' | ./bank)
check "Insufficient balance. Minimum balance Rs. 500.00 must remain. Available: 5000.00" \
      "withdraw that would break the minimum balance is refused"

OUT=$(printf '2\nCarol\n60000\n2468\n2468\n1\n1003\n2468\n2\n50000\n2\n1\n0\n0\n' | ./bank)
check "Withdrew 50000.00. New balance: 10000.00" "withdraw up to the daily limit works"
check "Daily withdrawal limit Rs. 50000.00 reached. Remaining today: 0.00" \
      "withdraw above the daily limit is refused"
OUT=$(printf '1\n1001\n4829\n3\n0\n0\n' | ./bank)
check "Balance for account 1001 (Asha): 5500.00" "balance after restart"

echo
echo "== data files =="
file_check "1001,Asha" data/accounts.txt "accounts.txt has the saved account"
file_check "DEPOSIT,1500.00" data/transactions.txt "transactions.txt has the deposit"
file_check "WITHDRAW,1000.00" data/transactions.txt "transactions.txt has the withdrawal"
secret_check "4829" "PIN is never stored in plain text"

echo
echo "== change PIN, weak-PIN policy (S2-03) =="
OUT=$(printf '1\n1001\n4829\n5\n4829\n7391\n7391\n5\n7391\n1111\n0\n0\n' | ./bank)
check "PIN changed" "PIN changed with the old PIN"
check "Weak PIN. Do not use 0000, 1111, 1234 or 4321." "weak PIN refused"

OUT=$(printf '1\n1001\n7391\n3\n0\n0\n' | ./bank)
check "Balance for account 1001 (Asha): 5500.00" "the new PIN works"

OUT=$(printf '1\n1001\n4829\n0\n' | ./bank)
check "Wrong PIN" "the old PIN no longer works"

secret_check "7391" "new PIN is not stored in plain text"

echo
if [ "$fail" -eq 0 ]; then
    echo "ALL CHECKS PASSED"
else
    echo "SOME CHECKS FAILED"
    exit 1
fi
