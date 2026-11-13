# Sprint 2 review and retrospective

**Delivered:** 11 of 11 items (34 story points), all merged through reviewed pull requests.
**Demo video:** `docs/sprint2/sprint2-demo.mp4`
**Tag:** `v1.0`
**Board:** all Sprint 2 cards are in Done.
**RTM:** `docs/sprint2/rtm.md`  ·  **Deviations:** `docs/sprint2/deviations.md`

## What the demo shows

Login with account 1001 / PIN 4829 -> 3 wrong PINs lock the account -> admin
unlocks it -> change PIN -> deposit (limit Rs. 2,00,000) -> withdraw with the
Rs. 500 minimum balance and the Rs. 50,000 daily limit -> mini statement ->
statement between two dates -> admin list with the total money, show one
account, close a zero-balance account -> restart keeps the data -> `make test`
all green -> `make perf` inside the SRS limits.

## What went well

- every change went through a branch, a pull request and a review by another member
- CI (GitHub Actions) builds and runs the smoke test on every push and pull request
- the data format was extended once (PIN hash, status, failed attempts) and every
  later feature built on it without a merge conflict
- atomic saves, backups and the append-only log make the data safe against a crash
- the performance run shows start-up about 0.05 s and operations far below the 1 s limit

## Problems

- Sprint 2 was compressed into five days, so two or three PRs had to be merged
  in a fixed order on some days
- the PIN and admin password use a simple, unsalted FNV-1a hash (documented in
  `deviations.md`; a salted hash is the first post-v1.0 improvement)
- money is stored as a decimal double, not as whole paise (kept from Sprint 1)
- the performance numbers were measured on the team's Mac, not on the CI runner

## Retrospective comments

The group's retro comments are in the comments of issue
[#23](https://github.com/Team-11-SE/Team11_Bank_Management_System/issues/23).

## Final state

- final tag `v1.0`, development frozen
- `docs/sprint2/` has the plan, performance evidence, RTM, deviations and review
- the 2-minute demo video is at `docs/sprint2/sprint2-demo.mp4`
