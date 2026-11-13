# Team 11 - Bank Management System

Mini-project for the Software Engineering course (Semester 5).
A small command-line bank program written in C.

## Team

| SRN | GitHub |
|---|---|
| PES1UG24CS703 | [@8figalltimepro](https://github.com/8figalltimepro) |
| PES1UG24CS707 | [@ganavigowda8343-ctrl](https://github.com/ganavigowda8343-ctrl) |
| PES1UG24CS711 | [@saanvisamayam](https://github.com/saanvisamayam) |

## What the program does (Sprint 2 scope)

- Customer login with account number + 4-digit PIN (the PIN is stored as a hash only)
- Account lockout after 3 wrong PINs; only the admin can unlock it
- Change PIN, with a weak-PIN check
- Deposit (limit Rs. 2,00,000 per deposit) and withdraw (Rs. 500 minimum balance,
  Rs. 50,000 per-day limit), plus balance inquiry
- Mini statement (last 10 transactions) and statement between two dates
- Admin menu: list all accounts and the total money, show one account, unlock a
  locked account, close a zero-balance account
- Atomic saves, an append-only transaction log, start-up backups (last 10 kept)
  and a data integrity check on every start
- Accounts in `data/accounts.txt`, transactions in `data/transactions.txt`

The demo admin password is `admin1234`; only its hash is in the binary.

## Build and run

```sh
make        # build the program -> ./bank
make run    # build and start the program
make test   # build and run the smoke test
make perf   # performance run: 10,000 accounts, 100,000 log lines (needs python3)
make clean  # remove the binary, the data files and the backups
```

Requires `gcc` (or `cc`) and `make`. Works on macOS and Linux.

## Project layout

| Path | What it holds |
|---|---|
| `src/main.c` | entry point |
| `src/ui_menu.c` | console menu and input handling |
| `src/accounts.c` | account list, validation, account creation |
| `src/transactions.c` | deposit, withdraw, balance, statement |
| `src/filestore.c` | reading and writing the data files |
| `include/bank.h` | shared struct and function declarations |
| `tests/` | smoke test |
| `tests/perf_test.py` | performance run and evidence generation |
| `docs/sprint1/` | sprint 1 plan, evidence, deviations, review, demo video |
| `docs/sprint2/` | sprint 2 plan, performance evidence, RTM, deviations, review |
