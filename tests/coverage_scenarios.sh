#!/usr/bin/env bash
set -euo pipefail

BANK=$1
WORK=$2

cd "$WORK"
mkdir -p data
fail=0

run() {
    local input=$1
    local description=$2
    local expected=$3
    local output

    output=$(printf '%b' "$input" | "$BANK")
    if printf '%s' "$output" | grep -Fq -- "$expected"; then
        echo "PASS: $description"
    else
        echo "FAIL: $description"
        fail=1
    fi
}

run '9\n0\n' \
    'unknown main menu option' \
    'Unknown option. Please choose between 0 and 3.'

run '2\nA\nB\n1\nAsha\n0\nabc\n4829\n1111\n4829\n4829\n0\n' \
    'create account retries and succeeds' \
    'Your account number is 1001.'

run '2\nBob\n1.234\n0\n1111\n4829\n4829\n0\n' \
    'invalid opening and weak PIN are refused' \
    'Weak PIN. Do not use 0000, 1111, 1234 or 4321.'

run '1\n9999\n4829\n1\n1001\n12\n1\n1001\n0000\n1\n1001\n4829\n3\n0\n0\n' \
    'login errors, wrong PIN and balance inquiry' \
    'Balance for account 1001 (Asha): 0.00'

run '1\n1001\n4829\n9\n1\n1000\n2\n10000\n3\n4\n5\n4829\n1111\n5\n4829\n8642\n0000\n5\n4829\n7391\n7391\n6\nabc\n2026-10-01\n2026-10-31\n0\n0\n' \
    'customer menu operations and PIN change retries' \
    'PINs do not match. PIN not changed.'

run '3\nwrongpass\n3\nadmin1234\n9\n1\n2\n9999\n2\n1001\n3\n1001\n3\n9999\n4\n1001\n4\n1002\n4\n1002\n0\n0\n' \
    'admin menu errors, list, show, unlock and close' \
    'Account 1002 is already CLOSED.'

run '2\nCara\n500\n4829\n4829\n1\n1003\n0000\n1\n1003\n0000\n1\n1003\n0000\n3\nadmin1234\n3\n1003\n0\n0\n' \
    'account lockout and admin unlock' \
    'Account 1003 unlocked.'

run '1\n1003\n0000\n1\n1003\n0000\n1\n1003\n0000\n1\n1003\n4829\n1\n1002\n4829\n0\n' \
    'locked and closed accounts refuse login' \
    'Account is LOCKED. Contact the bank admin.'

run '' \
    'EOF at the main menu exits cleanly' \
    'Bye!'

echo "CLI scenarios: $fail failures"
exit "$fail"
