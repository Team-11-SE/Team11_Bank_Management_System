# Tests

## Smoke test

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

## Coverage tests

```sh
make coverage
```

or directly:

```sh
bash tests/coverage.sh
```

This builds every `src/*.c` file with `--coverage`, runs two test drivers, and
uses `gcov` to produce both line coverage and branch coverage data:

- `tests/unit_tests.c` checks validation, hashing, login, PIN changes, account
  creation, deposits, withdrawals, statements, date checks and file storage.
  It also exercises error paths by making `data/accounts.tmp` and
  `data/transactions.txt` unwritable.
- `tests/coverage_scenarios.sh` drives the compiled CLI through menu retries,
  lockout, admin actions and EOF paths so `main.c` and `ui_menu.c` are covered.

The runner exits non-zero if fewer than 80% of lines or 70% of branches are
covered. Reports are written to `tests/.coverage/report/summary.md` and
`tests/.coverage/report/summary.json`; raw gcov data stays in
`tests/.coverage/`. All of those files are ignored by git.

Requires `gcc` (or `cc`), `make`, `python3` and `gcov`. If the compiler has a
versioned name, set `CC` and `GCOV` to match, for example
`CC=gcc-16 GCOV=gcov-16 make coverage`. Both LLVM `.gcov` and GCC
`.gcov.json.gz` data are accepted.

## Demo coverage summary

One run with GCC 16.2.0 (`CC=gcc-16 bash tests/coverage.sh`) produced:

| Source | Lines covered | Line coverage | Branches taken | Branch coverage |
|---|---:|---:|---:|---:|
| `accounts.c` | 119/119 | 100.00% | 90/94 | 95.74% |
| `filestore.c` | 99/115 | 86.09% | 58/76 | 76.32% |
| `main.c` | 8/8 | 100.00% | 0/0 | 100.00% |
| `transactions.c` | 104/104 | 100.00% | 82/84 | 97.62% |
| `ui_menu.c` | 267/326 | 81.90% | 132/198 | 66.67% |
| **Total** | **597/672** | **88.84%** | **362/452** | **80.09%** |

This is a snapshot; run `make coverage` to regenerate the current numbers in
`tests/.coverage/report/summary.md`.
