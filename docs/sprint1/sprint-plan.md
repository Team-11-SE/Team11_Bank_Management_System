# Sprint 1 plan - Team 11 Bank Management System

**Window:** Monday 19 Oct 2026 to Friday 23 Oct 2026 (1 week)
**Board:** https://github.com/orgs/Team-11-SE/projects/1
**Goal:** a working command-line bank program with the basic money operations:
create account, deposit, withdraw, balance and mini statement, with accounts and
transactions stored in files and every change reviewed through a pull request.

## Sprint backlog (11 items, 25 story points)

| Issue | Item | Owner | Points |
|---|---|---|---|
| [#2](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/2) | S1-01 Repo scaffolding and build system | @8figalltimepro | 2 |
| [#3](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/3) | S1-02 Account file storage and transaction log | @ganavigowda8343-ctrl | 3 |
| [#4](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/4) | S1-03 Main menu and input handling | @8figalltimepro | 2 |
| [#5](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/5) | S1-04 Create account with validation | @ganavigowda8343-ctrl | 3 |
| [#6](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/6) | S1-05 Deposit | @saanvisamayam | 3 |
| [#7](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/7) | S1-06 Withdraw with balance check | @saanvisamayam | 3 |
| [#8](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/8) | S1-07 Balance inquiry and mini statement | @saanvisamayam | 2 |
| [#9](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/9) | S1-08 Validation pass and data integrity | @ganavigowda8343-ctrl | 2 |
| [#10](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/10) | S1-09 CI workflow with GitHub Actions | @8figalltimepro | 2 |
| [#11](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/11) | S1-10 Smoke test script | @8figalltimepro | 2 |
| [#12](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/12) | S1-11 Sprint 1 documents, deviations and demo video | @8figalltimepro | 1 |

## Definition of done (every item)

- the code is merged into `main` through a pull request reviewed by another member
- `make` builds with no warnings on macOS and Linux
- the smoke test (`make test`) passes
- the card on the board is moved to Done

## Out of scope for Sprint 1 (planned in Sprint 2)

PIN login and security, admin menu, minimum balance and daily limits,
deposits limit, backups and full integrity check, atomic writes,
performance runs, final documentation and the 2-minute video (issues #13 to #23).

## How the week runs

- **Planning (Mon 19 Oct):** all Sprint 1 cards are moved from Backlog to Ready
- **Every evening:** each owner pushes their branch, opens a pull request, another
  member reviews it and merges it; cards move on the board the same evening
- **Review (Fri 23 Oct):** demo of the working product, 1-minute video, retro comments
  on issue [#12](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/12),
  tag `v0.1-sprint1`
