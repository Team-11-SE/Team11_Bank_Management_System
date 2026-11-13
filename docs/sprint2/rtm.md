# Sprint 2 requirements traceability matrix (RTM)

Status: **P = passed**. Every requirement below is implemented and was verified
with `make test`, `make perf` and the Sprint 2 demo (`docs/sprint2/review.md`).

| Req | Requirement | Module | Test case(s) | Status |
|---|---|---|---|---|
| BMS-F-001 | Customer login with account number and PIN | AuthModule | TC-Auth-01 | P |
| BMS-F-002 | Lock after 3 failed PIN attempts | AuthModule | TC-Auth-02 | P |
| BMS-F-003 | PIN stored as hash only | AuthModule | TC-Auth-03 | P |
| BMS-F-004 | Change PIN after old PIN check | AuthModule | TC-Auth-04 | P |
| BMS-F-005 | Logout ends the session | UiMenuModule | TC-Auth-05 | P |
| BMS-F-006 | Withdraw with minimum balance rule | AccountModule | TC-WD-01 | P |
| BMS-F-007 | Daily withdrawal limit Rs. 50,000 | TxnModule | TC-WD-02 | P |
| BMS-F-008 | Withdrawal log line before balance save | FileStoreModule | TC-WD-03 | P |
| BMS-F-009 | Deposit positive amount only | AccountModule | TC-DP-01 | P |
| BMS-F-010 | Single deposit limit Rs. 2,00,000 | AccountModule | TC-DP-02 | P |
| BMS-F-011 | Deposit log line | FileStoreModule | TC-DP-03 | P |
| BMS-F-012 | Admin login with password | AdminModule | TC-ADM-01 | P |
| BMS-F-013 | List accounts and total money | AdminModule | TC-ADM-02 | P |
| BMS-F-014 | Unlock locked account | AdminModule | TC-ADM-03 | P |
| BMS-F-015 | Close zero-balance account | AdminModule | TC-ADM-04 | P |
| BMS-F-016 | Timestamped backup on start-up | BackupModule | TC-MNT-01 | P |
| BMS-F-017 | Integrity check with line warning | FileStoreModule | TC-MNT-02 | P |
| BMS-F-018 | Create account with unique number | AccountModule | TC-ACC-01 | P |
| BMS-F-019 | Validate new-account input | AccountModule | TC-ACC-02 | P |
| BMS-F-020 | Show one account to admin | AdminModule | TC-ACC-03 | P |
| BMS-F-021 | Refuse weak PIN | AuthModule | TC-ACC-04 | P |
| BMS-F-022 | Balance inquiry | AccountModule | TC-INQ-01 | P |
| BMS-F-023 | Mini statement last 10 | ReportModule | TC-INQ-02 | P |
| BMS-F-024 | Statement between two dates | ReportModule | TC-INQ-03 | P |
| BMS-NF-001 | Response time <= 1 s (10,000 accounts) | AccountModule | TC-Perf-01 | P |
| BMS-NF-002 | Start-up <= 2 s | BackupModule | TC-Perf-02 | P |
| BMS-NF-003 | Mini statement <= 1 s | ReportModule | TC-Perf-03 | P |
| BMS-NF-004 | No money lost or created | AccountModule | TC-Rel-01 | P |
| BMS-NF-005 | Data file safe after a crash | FileStoreModule | TC-Rel-02 | P |
| BMS-NF-006 | Balance never negative | AccountModule | TC-Rel-03 | P |
| BMS-NF-007 | No plain-text secret in files | AuthModule | TC-Sec-01 | P |
| BMS-NF-008 | Lock after 3 failures | AuthModule | TC-Sec-02 | P |
| BMS-NF-009 | First-time user deposits in 60 s | UiMenuModule | TC-UX-01 | P |
| BMS-NF-010 | Warning-free build, 4 modules | All modules | TC-Mnt-01 | P |
| BMS-NF-011 | Capacity 10,000 accounts | FileStoreModule | TC-Cap-01 | P |
| PRJ-SR-001 | Hash-only secrets | AuthModule | TC-Sec-01 | P |
| PRJ-SR-002 | Account lock and admin unlock | AuthModule | TC-Sec-02 | P |
| PRJ-SR-003 | Atomic file write (temp + rename) | FileStoreModule | TC-Sec-03 | P |
| PRJ-SR-004 | Append-only transaction log | FileStoreModule | TC-Sec-04 | P |
| PRJ-SR-005 | No secret on screen or in logs | UiMenuModule | TC-Sec-05 | P |
| PRJ-SR-006 | Admin session control | AdminModule | TC-Sec-06 | P |
| PRJ-SR-007 | Amount validation and upper bound | AccountModule | TC-Sec-07 | P |
| SO-1 | Confidentiality of the PIN and the admin password | AuthModule | TC-Auth-03, TC-Sec-01 | P |
| SO-2 | Integrity of the money data | FileStoreModule | TC-Sec-03, TC-Rel-02 | P |
| SO-3 | Accountability and audit of every operation | FileStoreModule | TC-Sec-04, TC-WD-03 | P |
| SO-4 | Availability of the local data | FileStoreModule | TC-MNT-01, TC-MNT-02 | P |

**Summary:** 46 requirements (24 functional, 11 non-functional, 7 security,
4 security objectives) — all **P**.

Performance evidence: `docs/sprint2/perf-evidence.md`.
