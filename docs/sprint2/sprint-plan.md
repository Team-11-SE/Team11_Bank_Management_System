# Sprint 2 plan - Team 11 Bank Management System

**Window:** Saturday 7 November 2026 to Wednesday 11 November 2026 (final sprint)
**Board:** https://github.com/orgs/Team-11-SE/projects/1/views/2
**Goal:** deliver v1.0: PIN login and hashed PINs, lockout and admin unlock,
change PIN, deposit/withdrawal limits, admin menu, backups, integrity check,
atomic writes, date-range statement, performance evidence, final documents and
the 2-minute demo video. No Sprint 3.

## Backlog (11 items, 34 story points)

| Issue | Item | Owner | Points |
|---|---|---|---|
| [#13](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/13) | S2-01 PIN login, PIN hashing and sessions | @8figalltimepro | 5 |
| [#14](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/14) | S2-02 Account lockout after failed attempts | @ganavigowda8343-ctrl | 3 |
| [#15](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/15) | S2-03 Change PIN with PIN policy check | @saanvisamayam | 3 |
| [#16](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/16) | S2-04 Minimum balance and daily withdrawal limit | @saanvisamayam | 3 |
| [#17](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/17) | S2-05 Deposit limit and amount checks | @saanvisamayam | 2 |
| [#18](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/18) | S2-06 Admin menu: list, totals, unlock, close account | @ganavigowda8343-ctrl | 5 |
| [#19](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/19) | S2-07 Startup backup and full integrity check | @ganavigowda8343-ctrl | 3 |
| [#20](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/20) | S2-08 Atomic writes and append-only log | @8figalltimepro | 3 |
| [#21](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/21) | S2-09 Statement between two dates | @saanvisamayam | 2 |
| [#22](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/22) | S2-10 Performance run on 10,000 accounts | @8figalltimepro | 3 |
| [#23](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/23) | S2-11 Final documentation, 2-minute video, freeze | @8figalltimepro | 2 |

## Schedule

| Day | Task | Owner | Reviewer |
|---|---|---|---|
| Sat 7 Nov | S2-01 | Sam | Gowda |
| Sat 7 Nov | S2-02 | Gowda | Saanvi |
| Sun 8 Nov | S2-03 | Saanvi | Sam |
| Sun 8 Nov | S2-04 | Saanvi | Sam |
| Mon 9 Nov | S2-05 | Saanvi | Sam |
| Mon 9 Nov | S2-06 | Gowda | Saanvi |
| Tue 10 Nov | S2-07 | Gowda | Saanvi |
| Tue 10 Nov | S2-08 | Sam | Gowda |
| Tue 10 Nov | S2-09 | Saanvi | Sam |
| Wed 11 Nov | S2-10 | Sam | Gowda |
| Wed 11 Nov | S2-11 | Sam | Gowda |

## Definition of done (every item)

- one branch and one pull request per item, reviewed by another member, squash-merged
- `make` builds with no warnings and `make test` ends with `ALL CHECKS PASSED`
- CI check is green before the merge
- the card on the board moves In progress -> In review -> Done

## Final deliverables

- `docs/sprint2/` plan, performance evidence, RTM, deviations and review
- `docs/sprint2/sprint2-demo.mp4` (2-minute demo)
- tag `v1.0` and release notes
- 11/11 Sprint-2 cards in Done
