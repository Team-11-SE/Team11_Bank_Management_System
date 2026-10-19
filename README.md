# Team 11 - Bank Management System

Mini-project for the Software Engineering course (Semester 5).
A small command-line bank program written in C.

## Team

| SRN | GitHub |
|---|---|
| PES1UG24CS703 | [@8figalltimepro](https://github.com/8figalltimepro) |
| PES1UG24CS707 | [@ganavigowda8343-ctrl](https://github.com/ganavigowda8343-ctrl) |
| PES1UG24CS711 | [@saanvisamayam](https://github.com/saanvisamayam) |

## What the program does (Sprint 1 scope)

- Create an account (name + opening balance, account number is generated)
- Deposit and withdraw money (with a balance check)
- Balance inquiry and a mini statement (last 10 transactions)
- Accounts are stored in `data/accounts.txt`, transactions in `data/transactions.txt`

PIN login, admin functions, limits and backups are planned for Sprint 2
(see the issue board and the SRS document).

## Build and run

```sh
make        # build the program -> ./bank
make run    # build and start the program
make test   # build and run the smoke test
make clean  # remove the binary and the generated data files
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
| `docs/sprint1/` | sprint plan, evidence, deviations, review |
