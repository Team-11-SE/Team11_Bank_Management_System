#!/usr/bin/env bash
# Smoke test for the Team 11 Bank program (Sprint 1).
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

echo "== build =="
if ! make >/dev/null; then
    echo "FAIL: build"
    exit 1
fi
echo "build ok"

echo
echo "== run 1: create / deposit / refused withdraw / withdraw / balance / statement =="
OUT=$(printf '1\nAsha\n5000\n2\n1001\n1500\n3\n1001\n7000\n3\n1001\n1000\n4\n1001\n5\n1001\n0\n' | ./bank)

check "Your account number is 1001"                "account 1001 created"
check "New balance: 6500.00"                       "deposit 1500 -> balance 6500.00"
check "Insufficient balance. Available: 6500.00"   "withdraw of 7000 refused"
check "New balance: 5500.00"                       "withdraw 1000 -> balance 5500.00"
check "Balance for account 1001 (Asha): 5500.00"   "balance inquiry"
check "WITHDRAW,1000.00"                           "statement shows the withdrawal"
check "DEPOSIT,1500.00"                            "statement shows the deposit"

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
echo "== run 2: data still there after a restart =="
OUT=$(printf '4\n1001\n0\n' | ./bank)
check "Balance for account 1001 (Asha): 5500.00"   "balance after restart"

echo
echo "== data files =="
file_check "1001,Asha,5500.00" data/accounts.txt    "accounts.txt has the saved account"
file_check "DEPOSIT,1500.00" data/transactions.txt  "transactions.txt has the deposit"
file_check "WITHDRAW,1000.00" data/transactions.txt "transactions.txt has the withdrawal"

echo
if [ "$fail" -eq 0 ]; then
    echo "ALL CHECKS PASSED"
else
    echo "SOME CHECKS FAILED"
    exit 1
fi
