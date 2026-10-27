# Sprint 1 deviations from the submitted documents (SRS, SAD, Test Plan)

These are the known differences between the submitted documents and what
Sprint 1 actually delivers. They will be explained during the final demo
(course requirement, Part-2 item 2.i).

| # | Document says | Sprint 1 has | Why | Planned for |
|---|---|---|---|---|
| 1 | Account creation asks for a 4-digit PIN (BMS-F-018, BMS-F-019) | name + opening balance only; the account number is generated | PIN needs hashing, lockout and its own tests, done as one security piece | Sprint 2 (S2-01, S2-03) |
| 2 | Withdrawal keeps Rs. 500 minimum balance (BMS-F-006) and a Rs. 50,000 daily limit (BMS-F-007) | withdraw only refuses amounts above the balance (no overdraft) | basic money flow first; the two rules come with their own acceptance tests | Sprint 2 (S2-04) |
| 3 | Single deposit above Rs. 2,00,000 is refused (BMS-F-010) | any amount above 0 with at most 2 decimals is accepted (hard cap kept at 10 crore) | limit constant not wired yet | Sprint 2 (S2-05) |
| 4 | Money is stored as a whole number of paise (SRS 1.4) | money is stored as a decimal number with 2 decimal places and rounded | keeps Sprint 1 simple; every amount is validated and shown with 2 decimals | to review in Sprint 2 |
| 5 | Admin menu, listing, unlock, close, backups (BMS-F-012 to BMS-F-016) | not present | Sprint 1 covers customer operations only | Sprint 2 (S2-06, S2-07) |
| 6 | PIN stored as a hash, lockout, atomic writes (PRJ-SR-001 to PRJ-SR-005) | not present | security work planned as one piece in Sprint 2 | Sprint 2 (S2-01, S2-02, S2-08) |

Everything else follows the SRS: account numbers start at 1001 and never repeat,
the transaction log is append-only, the mini statement shows the last 10
transactions newest first, and a damaged data line is skipped with a warning.
