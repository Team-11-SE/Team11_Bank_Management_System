# Smoke test

One command to check the basic flow (create account, deposit, refused withdraw,
valid withdraw, balance, statement, restart).

```sh
make test
```

or directly:

```sh
bash tests/smoke_test.sh
```

The script builds the program, wipes `data/accounts.txt` and
`data/transactions.txt`, runs the program with a fixed input and checks the
output and the data files. Every check prints PASS or FAIL, and the script
ends with a non-zero exit code if anything failed.

Test cases covered (see the test plan): TC-ACC-01, TC-DP-01, TC-WD-01,
TC-INQ-01, TC-INQ-02.
