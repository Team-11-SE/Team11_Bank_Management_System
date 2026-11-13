# Sprint 2 deviations from the submitted documents (SRS, SAD, Test Plan)

These are the known differences between the submitted documents and the
delivered v1.0 program. They are explained during the final demo
(course requirement, Part-2 item 3.d).

| # | Document says | v1.0 has | Why | Planned |
|---|---|---|---|---|
| 1 | Money is stored as a whole number of paise (SRS 1.4) | stored as a decimal number with 2 decimal places and rounded | keeps the Sprint 1 code stable; every amount is validated and printed with 2 decimals | after v1.0 |
| 2 | PIN and admin password are stored as salted hashes (PRJ-SR-001) | simple unsalted FNV-1a hash, never plain text | the SAD allows a simple hash of the standard library; a salted hash needs its own tests | after v1.0 |
| 3 | File locking so two copies cannot share one data folder (SAD 4.6) | one program instance at a time; no lock file | small class project, single user at a time | after v1.0 |
| 4 | Admin password is provisioned by the bank | fixed hash constant; the demo password is `admin1234` | there is no user-admin provisioning in the course scope | after v1.0 |
| 5 | Timestamps are local wall-clock time | local time, `YYYY-MM-DD HH:MM:SS`, no timezone | single-machine demo | not planned |
| 6 | Performance measured on the test machine | measured on macOS (Apple Silicon); CI is Linux | the SRS asks for the team's test machine | re-run on the review machine |
| 7 | Optional PIN entry with no echo (SAD 4.3.7) | no echo when stdin is a terminal; piped input (tests) reads normally | the smoke test and CI need piped input | not planned |
