# Sprint 1 review and retrospective

**Delivered:** 11 of 11 items (25 story points), all merged through reviewed pull requests.
**Demo video:** `docs/sprint1/sprint1-demo.mp4`
**Board:** all Sprint 1 cards are in Done (see the "Current iteration" view).

## What the demo shows

Create account -> 1001 -> deposit 1500 (balance 6500) -> withdraw 7000 refused ->
withdraw 1000 (balance 5500) -> balance -> mini statement (newest first) ->
restart keeps the data -> `make test` all green.

## What went well

- every change went through a branch, a pull request and a review by another member
- CI (GitHub Actions) builds and runs the smoke test on every push and pull request
- the module split from the SAD (menu, accounts, transactions, filestore) held up:
  each member touched only their own files, so there were no merge conflicts
- the smoke test was added mid-sprint, so from Thursday we could check the full
  flow with one command

## Problems

- Thursday (Day 4) had three pull requests in one evening; next time spread the work
  more evenly across the week
- Sprint 1 has no PIN login yet (see `deviations.md`) - it is the very first item
  of Sprint 2
- everyone must remember to `git pull` before starting their evening work

## Retrospective comments

The group's retro comments are in the comments of issue
[#12](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/12).

## Next sprint (planned)

Issues #13 to #23 on the board: PIN login and security, admin menu,
limits, backups and integrity, atomic writes, performance run,
final documentation and the 2-minute video (Sprint 2: 26-30 Oct).
