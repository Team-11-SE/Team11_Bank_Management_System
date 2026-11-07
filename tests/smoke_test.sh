#!/usr/bin/env bash
# Smoke test for the Team 11 Bank program (Sprint 2).
# Run from the repo root with:  make test   (or: bash tests/smoke_test.sh)
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
if grep -Eq '^1001,Asha,[0-9a-f]{16},5000\.00$' data/accounts.txt 2>/dev/null; then
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
OUT=$(printf '1\n1001\n4829\n3\n0\n0\n' | ./bank)
check "Balance for account 1001 (Asha): 5500.00" "balance after restart"

echo
echo "== data files =="
file_check "1001,Asha" data/accounts.txt "accounts.txt has the saved account"
file_check "DEPOSIT,1500.00" data/transactions.txt "transactions.txt has the deposit"
file_check "WITHDRAW,1000.00" data/transactions.txt "transactions.txt has the withdrawal"
secret_check "4829" "PIN is never stored in plain text"

echo
if [ "$fail" -eq 0 ]; then
    echo "ALL CHECKS PASSED"
else
    echo "SOME CHECKS FAILED"
    exit 1
fi
