#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "bank.h"

extern char *mkdtemp(char *template_path);

static int checks_run;
static int checks_failed;
static char workdir[PATH_MAX];

#define CHECK(cond) do {                                                   \
    checks_run++;                                                          \
    if (!(cond)) {                                                         \
        printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);             \
        checks_failed++;                                                   \
    }                                                                      \
} while (0)

#define CHECK_STR(actual, expected) do {                                   \
    checks_run++;                                                          \
    if (strcmp((actual), (expected)) != 0) {                               \
        printf("FAIL %s:%d: expected \"%s\", got \"%s\"\n",                \
               __FILE__, __LINE__, (expected), (actual));                  \
        checks_failed++;                                                   \
    }                                                                      \
} while (0)

static void die(const char *msg)
{
    perror(msg);
    exit(2);
}

static void remove_tree(const char *path)
{
    DIR *dir = opendir(path);
    struct dirent *ent;

    if (dir == NULL)
        return;
    while ((ent = readdir(dir)) != NULL) {
        char child[PATH_MAX];
        struct stat st;

        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        snprintf(child, sizeof child, "%s/%s", path, ent->d_name);
        if (lstat(child, &st) == 0 && S_ISDIR(st.st_mode))
            remove_tree(child);
        else
            remove(child);
    }
    closedir(dir);
    rmdir(path);
}

static void make_workdir(void)
{
    const char *base = getenv("TMPDIR");
    char tmpl[256];

    snprintf(tmpl, sizeof tmpl, "%s/bank-coverage-XXXXXX",
             base != NULL ? base : "/tmp");
    if (mkdtemp(tmpl) == NULL)
        die("mkdtemp");
    snprintf(workdir, sizeof workdir, "%s", tmpl);
    if (chdir(workdir) != 0)
        die("chdir");
    if (mkdir("data", 0755) != 0 && errno != EEXIST)
        die("mkdir data");
}

static void reset_data(void)
{
    remove("data/accounts.txt");
    remove("data/transactions.txt");
    remove("data/accounts.tmp");
    remove_tree("data/backup");
}

static void write_text(const char *path, const char *text)
{
    FILE *fp = fopen(path, "w");

    if (fp == NULL)
        die(path);
    fputs(text, fp);
    fclose(fp);
}

static void read_text(const char *path, char *buf, size_t size)
{
    FILE *fp = fopen(path, "r");
    size_t n;

    buf[0] = '\0';
    if (fp == NULL)
        return;
    n = fread(buf, 1, size - 1, fp);
    buf[n] = '\0';
    fclose(fp);
}

static int contains(const char *haystack, const char *needle)
{
    return strstr(haystack, needle) != NULL;
}

static void today(char *out, size_t size)
{
    time_t now = time(NULL);
    strftime(out, size, "%Y-%m-%d", localtime(&now));
}

static void capture_start(FILE **capture, int *saved)
{
    fflush(stdout);
    *saved = dup(STDOUT_FILENO);
    *capture = tmpfile();
    if (*saved < 0 || *capture == NULL)
        die("capture_start");
    dup2(fileno(*capture), STDOUT_FILENO);
}

static void capture_end(FILE *capture, int saved, char *buf, size_t size)
{
    size_t n;

    fflush(stdout);
    dup2(saved, STDOUT_FILENO);
    close(saved);
    rewind(capture);
    n = fread(buf, 1, size - 1, capture);
    buf[n] = '\0';
    fclose(capture);
}

static void init_account(struct account *a, int no, const char *name,
                         const char *pin, double balance, const char *status)
{
    memset(a, 0, sizeof *a);
    a->acc_no = no;
    snprintf(a->name, NAME_LEN, "%s", name);
    auth_hash(pin, a->pin_hash);
    a->balance = balance;
    snprintf(a->status, STATUS_LEN, "%s", status);
}

static void block_accounts_save(void)
{
    if (mkdir("data/accounts.tmp", 0755) != 0)
        die("mkdir accounts.tmp");
}

static void unblock_accounts_save(void)
{
    if (rmdir("data/accounts.tmp") != 0)
        die("rmdir accounts.tmp");
}

static void block_txn_log(void)
{
    remove("data/transactions.txt");
    if (mkdir("data/transactions.txt", 0755) != 0)
        die("mkdir transactions.txt");
}

static void unblock_txn_log(void)
{
    if (rmdir("data/transactions.txt") != 0)
        die("rmdir transactions.txt");
}

static void test_acc_find(void)
{
    struct account list[3];

    init_account(&list[0], 1001, "Asha", "4829", 1000.0, "ACTIVE");
    init_account(&list[1], 1002, "Bob", "3456", 2000.0, "ACTIVE");
    init_account(&list[2], 1003, "Cara", "7391", 3000.0, "LOCKED");

    CHECK(acc_find(list, 0, 1001) == -1);
    CHECK(acc_find(list, 3, 1001) == 0);
    CHECK(acc_find(list, 3, 1002) == 1);
    CHECK(acc_find(list, 3, 1003) == 2);
    CHECK(acc_find(list, 3, 9999) == -1);
}

static void test_acc_next_number(void)
{
    struct account list[3];

    init_account(&list[0], 1001, "Asha", "4829", 1000.0, "ACTIVE");
    init_account(&list[1], 1500, "Bob", "3456", 2000.0, "ACTIVE");
    init_account(&list[2], 1200, "Cara", "7391", 3000.0, "ACTIVE");

    CHECK(acc_next_number(NULL, 0) == 1001);
    CHECK(acc_next_number(list, 3) == 1501);
    init_account(&list[0], 500, "Asha", "4829", 1000.0, "ACTIVE");
    init_account(&list[1], 1000, "Bob", "3456", 2000.0, "ACTIVE");
    CHECK(acc_next_number(list, 2) == 1001);
}

static void test_acc_valid_name(void)
{
    char long_name[NAME_LEN + 1];

    memset(long_name, 'a', NAME_LEN);
    long_name[NAME_LEN] = '\0';

    CHECK(acc_valid_name("Asha") == 1);
    CHECK(acc_valid_name("Mary Jane") == 1);
    CHECK(acc_valid_name("Al") == 1);
    CHECK(acc_valid_name("A B") == 1);
    CHECK(acc_valid_name("") == 0);
    CHECK(acc_valid_name("A") == 0);
    CHECK(acc_valid_name(long_name) == 0);
    CHECK(acc_valid_name("1Ab") == 0);
    CHECK(acc_valid_name("Ab1") == 0);
    CHECK(acc_valid_name("A  B") == 0);
    CHECK(acc_valid_name("A-B") == 0);
}

static void test_acc_valid_amount(void)
{
    CHECK(acc_valid_amount(1.00) == 1);
    CHECK(acc_valid_amount(0.01) == 1);
    CHECK(acc_valid_amount(0.10) == 1);
    CHECK(acc_valid_amount(1234.56) == 1);
    CHECK(acc_valid_amount(9999999.99) == 1);
    CHECK(acc_valid_amount(0.0) == 0);
    CHECK(acc_valid_amount(-5.0) == 0);
    CHECK(acc_valid_amount(10000000.0) == 0);
    CHECK(acc_valid_amount(1.234) == 0);
    CHECK(acc_valid_amount(1.005) == 0);
}

static void test_acc_valid_opening(void)
{
    CHECK(acc_valid_opening(0.0) == 1);
    CHECK(acc_valid_opening(500.0) == 1);
    CHECK(acc_valid_opening(-1.0) == 0);
    CHECK(acc_valid_opening(1.234) == 0);
}

static void test_auth_hash_and_pin_policy(void)
{
    char hash[HASH_LEN];

    auth_hash("4829", hash);
    CHECK_STR(hash, "55f39b5145752c12");
    auth_hash("", hash);
    CHECK_STR(hash, "14650fb0739d0383");
    auth_hash("1234", hash);
    CHECK_STR(hash, "dfed542765c39e5b");

    CHECK(auth_valid_pin("4829") == 1);
    CHECK(auth_valid_pin("") == 0);
    CHECK(auth_valid_pin("123") == 0);
    CHECK(auth_valid_pin("12345") == 0);
    CHECK(auth_valid_pin("12a4") == 0);

    CHECK(auth_valid_pin_policy("4829") == 1);
    CHECK(auth_valid_pin_policy("1212") == 1);
    CHECK(auth_valid_pin_policy("0000") == 0);
    CHECK(auth_valid_pin_policy("1111") == 0);
    CHECK(auth_valid_pin_policy("1234") == 0);
    CHECK(auth_valid_pin_policy("4321") == 0);
    CHECK(auth_valid_pin_policy("12a4") == 0);
}

static void test_auth_login(void)
{
    struct account list[4];
    int idx = -1;
    char saved[LINE_LEN];

    reset_data();
    init_account(&list[0], 1001, "Asha", "4829", 5000.0, "ACTIVE");
    init_account(&list[1], 1002, "Bob", "3456", 2000.0, "LOCKED");
    init_account(&list[2], 1003, "Cara", "7391", 3000.0, "CLOSED");
    init_account(&list[3], 1004, "Dave", "5678", 4000.0, "ACTIVE");

    CHECK(auth_login(list, 4, 9999, "4829", &idx) == 1);
    CHECK(auth_login(list, 4, 1002, "3456", &idx) == 3);
    CHECK(auth_login(list, 4, 1003, "7391", &idx) == 4);
    idx = -1;
    CHECK(auth_login(list, 4, 1001, "12", &idx) == 2);
    CHECK(list[0].bad_tries == 0);

    CHECK(auth_login(list, 4, 1001, "0000", &idx) == 2);
    CHECK(idx == 0);
    CHECK(list[0].bad_tries == 1);
    CHECK(auth_login(list, 4, 1001, "0000", &idx) == 2);
    CHECK(auth_login(list, 4, 1001, "0000", &idx) == 2);
    CHECK(list[0].bad_tries == 3);
    CHECK(strcmp(list[0].status, "LOCKED") == 0);
    CHECK(auth_login(list, 4, 1001, "4829", &idx) == 3);

    init_account(&list[0], 1001, "Asha", "4829", 5000.0, "ACTIVE");
    CHECK(auth_login(list, 4, 1001, "4829", &idx) == 0);
    CHECK(idx == 0);
    read_text("data/accounts.txt", saved, sizeof saved);
    CHECK(contains(saved, "1001,Asha,"));

    block_accounts_save();
    CHECK(auth_login(list, 4, 1004, "0000", &idx) == 5);
    unblock_accounts_save();
}

static void test_acc_change_pin(void)
{
    struct account list[2];
    char hash[HASH_LEN];

    reset_data();
    init_account(&list[0], 1001, "Asha", "4829", 5000.0, "ACTIVE");
    init_account(&list[1], 1002, "Bob", "3456", 2000.0, "ACTIVE");

    CHECK(acc_change_pin(list, 2, -1, "4829", "7391") == 1);
    CHECK(acc_change_pin(list, 2, 2, "4829", "7391") == 1);
    CHECK(acc_change_pin(list, 2, 0, "12", "7391") == 1);
    CHECK(acc_change_pin(list, 2, 0, "0000", "7391") == 1);
    CHECK(acc_change_pin(list, 2, 0, "4829", "1111") == 2);
    CHECK(acc_change_pin(list, 2, 0, "4829", "7391") == 0);
    auth_hash("7391", hash);
    CHECK(strcmp(list[0].pin_hash, hash) == 0);

    block_accounts_save();
    CHECK(acc_change_pin(list, 2, 1, "3456", "8642") == 3);
    unblock_accounts_save();
}

static void test_acc_create(void)
{
    struct account list[MAX_ACCOUNTS];
    int count = 0;
    char saved[LINE_LEN];

    reset_data();
    memset(list, 0, sizeof list);

    CHECK(acc_create(list, &count, "A", 100.0, "4829") == -1);
    CHECK(acc_create(list, &count, "Asha", 1.234, "4829") == -2);
    CHECK(acc_create(list, &count, "Asha", 100.0, "12ab") == -5);
    count = MAX_ACCOUNTS;
    CHECK(acc_create(list, &count, "Asha", 100.0, "4829") == -3);
    count = 0;

    CHECK(acc_create(list, &count, "Asha", 5000.0, "4829") == 1001);
    CHECK(count == 1);
    CHECK(list[0].acc_no == 1001);
    CHECK(list[0].balance == 5000.0);
    CHECK(strcmp(list[0].status, "ACTIVE") == 0);
    CHECK(list[0].bad_tries == 0);
    read_text("data/accounts.txt", saved, sizeof saved);
    CHECK(contains(saved, "1001,Asha,"));

    CHECK(acc_create(list, &count, "Bob", 0.0, "3456") == 1002);
    CHECK(list[1].balance == 0.0);

    block_accounts_save();
    CHECK(acc_create(list, &count, "Cara", 100.0, "7391") == -4);
    unblock_accounts_save();
}

static void test_txn_deposit(void)
{
    struct account list[2];
    char saved[LINE_LEN];

    reset_data();
    init_account(&list[0], 1001, "Asha", "4829", 5000.0, "ACTIVE");
    init_account(&list[1], 1002, "Bob", "3456", 2000.0, "LOCKED");

    CHECK(txn_deposit(list, 2, 9999, 100.0) == 1);
    CHECK(txn_deposit(list, 2, 1001, 1.234) == 2);
    CHECK(txn_deposit(list, 2, 1002, 100.0) == 6);
    CHECK(txn_deposit(list, 2, 1001, 200000.01) == 7);
    CHECK(txn_deposit(list, 2, 1001, 1500.0) == 0);
    CHECK(list[0].balance == 6500.0);
    read_text("data/transactions.txt", saved, sizeof saved);
    CHECK(contains(saved, ",1001,DEPOSIT,1500.00,6500.00,"));

    block_txn_log();
    CHECK(txn_deposit(list, 2, 1001, 100.0) == 4);
    unblock_txn_log();

    block_accounts_save();
    CHECK(txn_deposit(list, 2, 1001, 100.0) == 4);
    unblock_accounts_save();
}

static void test_txn_withdraw(void)
{
    struct account list[3];
    char log[LINE_LEN];
    char date[11];

    reset_data();
    init_account(&list[0], 1001, "Asha", "4829", 5000.0, "ACTIVE");
    init_account(&list[1], 1002, "Bob", "3456", 60000.0, "ACTIVE");
    init_account(&list[2], 1003, "Cara", "7391", 2000.0, "LOCKED");
    today(date, sizeof date);

    CHECK(txn_withdraw(list, 3, 9999, 100.0) == 1);
    CHECK(txn_withdraw(list, 3, 1001, 1.234) == 2);
    CHECK(txn_withdraw(list, 3, 1003, 100.0) == 6);
    CHECK(txn_withdraw(list, 3, 1001, 4501.0) == 3);
    CHECK(txn_withdraw(list, 3, 1001, 1000.0) == 0);
    CHECK(list[0].balance == 4000.0);

    reset_data();
    init_account(&list[1], 1002, "Bob", "3456", 60000.0, "ACTIVE");
    snprintf(log, sizeof log,
             "1,1002,WITHDRAW,50000.00,50000.00,%s 10:00:00\n", date);
    write_text("data/transactions.txt", log);
    CHECK(txn_withdraw(list, 3, 1002, 1.00) == 5);
    CHECK(list[1].balance == 60000.0);

    block_txn_log();
    CHECK(txn_withdraw(list, 3, 1001, 100.0) == 4);
    unblock_txn_log();

    block_accounts_save();
    CHECK(txn_withdraw(list, 3, 1001, 100.0) == 4);
    unblock_accounts_save();
}

static void test_txn_balance_and_statements(void)
{
    struct account list[1];
    char out[LINE_LEN * 8];
    char log[LINE_LEN * 4];
    char buf[LINE_LEN];
    char date[11];
    FILE *capture;
    int saved;
    int i;

    reset_data();
    init_account(&list[0], 1001, "Asha", "4829", 5000.0, "ACTIVE");

    capture_start(&capture, &saved);
    txn_balance(list, 1, 1001);
    capture_end(capture, saved, out, sizeof out);
    CHECK(contains(out, "Balance for account 1001 (Asha): 5000.00"));

    capture_start(&capture, &saved);
    txn_balance(list, 1, 9999);
    capture_end(capture, saved, out, sizeof out);
    CHECK(contains(out, "Account 9999 not found."));

    capture_start(&capture, &saved);
    txn_print_last(1001, 10);
    capture_end(capture, saved, out, sizeof out);
    CHECK(contains(out, "No transactions found for account 1001."));

    log[0] = '\0';
    for (i = 1; i <= 13; i++) {
        snprintf(buf, sizeof buf, "%d,1001,DEPOSIT,%d.00,%d.00,2026-10-27 10:00:00\n",
                 i, i, i);
        strncat(log, buf, sizeof log - strlen(log) - 1);
    }
    snprintf(buf, sizeof buf, "1,1002,DEPOSIT,99.00,99.00,2026-10-27 10:00:00\n");
    strncat(log, buf, sizeof log - strlen(log) - 1);
    write_text("data/transactions.txt", log);

    capture_start(&capture, &saved);
    txn_print_last(1001, 10);
    capture_end(capture, saved, out, sizeof out);
    CHECK(contains(out, "Last 10 transaction(s) for account 1001, newest first"));
    CHECK(contains(out, "13,1001,DEPOSIT,13.00"));
    CHECK(!contains(out, "1,1001,DEPOSIT,1.00,"));
    CHECK(!contains(out, "1002,DEPOSIT,99.00"));

    capture_start(&capture, &saved);
    txn_print_last(1001, 0);
    capture_end(capture, saved, out, sizeof out);
    CHECK(contains(out, "Last 1 transaction(s)"));
    CHECK(contains(out, "13,1001,DEPOSIT,13.00"));
    CHECK(!contains(out, "12,1001,DEPOSIT,12.00"));

    capture_start(&capture, &saved);
    txn_print_last(1001, 99);
    capture_end(capture, saved, out, sizeof out);
    CHECK(contains(out, "Last 10 transaction(s)"));

    capture_start(&capture, &saved);
    txn_print_last(9999, 10);
    capture_end(capture, saved, out, sizeof out);
    CHECK(contains(out, "No transactions found for account 9999."));

    reset_data();
    CHECK(txn_withdrawn_today(1001) == 0.0);

    today(date, sizeof date);
    snprintf(log, sizeof log,
             "1,1001,WITHDRAW,100.00,900.00,%s 10:00:00\n"
             "2,1001,WITHDRAW,250.50,649.50,%s 11:00:00\n"
             "3,1001,WITHDRAW,999.00,9.00,2020-01-01 10:00:00\n"
             "4,1001,DEPOSIT,500.00,1149.50,%s 12:00:00\n"
             "5,1002,WITHDRAW,700.00,300.00,%s 13:00:00\n",
             date, date, date, date);
    write_text("data/transactions.txt", log);
    CHECK(txn_withdrawn_today(1001) == 350.5);
    CHECK(txn_withdrawn_today(1002) == 700.0);
    CHECK(txn_withdrawn_today(9999) == 0.0);
}

static void test_txn_date_valid(void)
{
    CHECK(txn_date_valid("2026-10-27") == 1);
    CHECK(txn_date_valid("2000-01-01") == 1);
    CHECK(txn_date_valid("2100-12-31") == 1);
    CHECK(txn_date_valid("2026-2-01") == 0);
    CHECK(txn_date_valid("2026-12-31x") == 0);
    CHECK(txn_date_valid("abcd-01-01") == 0);
    CHECK(txn_date_valid("1999-01-01") == 0);
    CHECK(txn_date_valid("2101-01-01") == 0);
    CHECK(txn_date_valid("2026-00-01") == 0);
    CHECK(txn_date_valid("2026-13-01") == 0);
    CHECK(txn_date_valid("2026-01-00") == 0);
    CHECK(txn_date_valid("2026-01-32") == 0);
    CHECK(txn_date_valid("2026-02-30") == 1);
}

static void test_txn_print_range(void)
{
    char out[LINE_LEN * 8];
    FILE *capture;
    int saved;

    reset_data();
    capture_start(&capture, &saved);
    txn_print_range(1001, "2026-10-01", "2026-10-31");
    capture_end(capture, saved, out, sizeof out);
    CHECK(contains(out, "No transactions in this period"));

    write_text("data/transactions.txt",
               "1,1001,DEPOSIT,100.00,5100.00,2026-10-01 10:00:00\n"
               "2,1001,WITHDRAW,50.00,5050.00,2026-10-15 11:00:00\n"
               "3,1001,DEPOSIT,20.00,5070.00,2026-11-01 12:00:00\n"
               "4,1002,DEPOSIT,30.00,3030.00,2026-10-10 10:00:00\n"
               "garbage line\n");

    capture_start(&capture, &saved);
    txn_print_range(1001, "2026-10-01", "2026-10-31");
    capture_end(capture, saved, out, sizeof out);
    CHECK(contains(out, "Statement for account 1001 from 2026-10-01 to 2026-10-31"));
    CHECK(contains(out, "2 transaction(s) in this period."));
    CHECK(contains(out, "2026-10-01 10:00:00"));
    CHECK(contains(out, "2026-10-15 11:00:00"));
    CHECK(!contains(out, "2026-11-01 12:00:00"));
    CHECK(!contains(out, "1002,DEPOSIT"));

    capture_start(&capture, &saved);
    txn_print_range(1001, "2027-01-01", "2027-01-31");
    capture_end(capture, saved, out, sizeof out);
    CHECK(contains(out, "No transactions in this period"));
}

static void test_filestore_load_save(void)
{
    struct account list[4];
    char saved[LINE_LEN * 4];
    int count;

    reset_data();
    CHECK(fs_load(list, 4) == 0);

    write_text("data/accounts.txt",
               "1001,Asha,55f39b5145752c12,5000.00,ACTIVE,0\n"
               "garbage line\n"
               "1002,Bob,55f39b5145752c12,2000.00,LOCKED,3\n");

    {
        FILE *capture;
        char out[LINE_LEN * 2];
        int stdout_saved;

        capture_start(&capture, &stdout_saved);
        count = fs_load(list, 4);
        capture_end(capture, stdout_saved, out, sizeof out);
        CHECK(count == 2);
        CHECK(contains(out, "line 2 is damaged"));
    }

    CHECK(list[0].acc_no == 1001);
    CHECK(list[0].balance == 5000.0);
    CHECK(strcmp(list[0].status, "ACTIVE") == 0);
    CHECK(list[1].acc_no == 1002);
    CHECK(list[1].bad_tries == 3);
    CHECK(fs_load(list, 1) == 1);

    init_account(&list[0], 1001, "Asha", "4829", 5000.0, "ACTIVE");
    init_account(&list[1], 1002, "Bob", "3456", 2000.0, "ACTIVE");
    CHECK(fs_save(list, 2) == 0);
    read_text("data/accounts.txt", saved, sizeof saved);
    CHECK(contains(saved, "1001,Asha,"));
    CHECK(contains(saved, "1002,Bob,"));
    CHECK(fs_load(list, 4) == 2);

    block_accounts_save();
    CHECK(fs_save(list, 2) == 1);
    unblock_accounts_save();
}

static void test_filestore_log_and_backup(void)
{
    char saved[LINE_LEN * 4];
    char out[LINE_LEN * 2];
    char name[64];
    struct account empty;
    FILE *capture;
    DIR *dir;
    struct dirent *ent;
    int stdout_saved;
    int backups = 0;
    int current_backups = 0;
    int i;

    reset_data();
    write_text("data/accounts.txt", "1001,Asha,hash,5000.00,ACTIVE,0\n");
    write_text("data/transactions.txt", "1,1001,DEPOSIT,100.00,5100.00,2026-10-27 10:00:00\n");

    CHECK(fs_log_txn(1001, "WITHDRAW", 50.0, 5050.0) == 0);
    CHECK(fs_log_txn(1001, "WITHDRAW", 25.0, 5025.0) == 0);
    read_text("data/transactions.txt", saved, sizeof saved);
    CHECK(contains(saved, "2,1001,WITHDRAW,50.00,5050.00,"));
    CHECK(contains(saved, "3,1001,WITHDRAW,25.00,5025.00,"));

    block_txn_log();
    CHECK(fs_log_txn(1001, "DEPOSIT", 10.0, 5035.0) == 1);
    unblock_txn_log();

    reset_data();
    capture_start(&capture, &stdout_saved);
    fs_check_transactions();
    capture_end(capture, stdout_saved, out, sizeof out);
    CHECK(!contains(out, "Warning"));

    write_text("data/transactions.txt",
               "1,1001,DEPOSIT,100.00,5100.00,2026-10-27 10:00:00\n"
               "garbage line\n"
               "2,1001,WITHDRAW,50.00,5050.00,2026-10-27 11:00:00\n");
    capture_start(&capture, &stdout_saved);
    fs_check_transactions();
    capture_end(capture, stdout_saved, out, sizeof out);
    CHECK(contains(out, "Checked 2 transaction line(s)."));
    CHECK(contains(out, "line 2 is damaged"));

    reset_data();
    if (mkdir("data/backup", 0755) != 0 && errno != EEXIST)
        die("mkdir backup");
    for (i = 1; i <= 12; i++) {
        snprintf(name, sizeof name, "data/backup/accounts_2000-01-01_%04d.txt", i);
        write_text(name, "old\n");
        snprintf(name, sizeof name, "data/backup/transactions_2000-01-01_%04d.txt", i);
        write_text(name, "old\n");
    }
    write_text("data/accounts.txt", "1001,Asha,hash,5000.00,ACTIVE,0\n");
    write_text("data/transactions.txt", "1,1001,DEPOSIT,100.00,5100.00,2026-10-27 10:00:00\n");
    fs_startup_backup();

    dir = opendir("data/backup");
    if (dir == NULL)
        die("opendir backup");
    while ((ent = readdir(dir)) != NULL) {
        if (strstr(ent->d_name, ".txt") != NULL)
            backups++;
    }
    closedir(dir);
    CHECK(backups == 20);

    dir = opendir("data/backup");
    if (dir == NULL)
        die("opendir backup");
    while ((ent = readdir(dir)) != NULL) {
        char path[PATH_MAX * 2];

        snprintf(path, sizeof path, "data/backup/%s", ent->d_name);
        if (contains(ent->d_name, "accounts_")) {
            read_text(path, saved, sizeof saved);
            if (contains(saved, "1001,Asha"))
                current_backups++;
        }
    }
    closedir(dir);
    CHECK(current_backups == 1);
    CHECK(fs_save(&empty, 0) == 0);
}

int main(void)
{
    make_workdir();

    test_acc_find();
    test_acc_next_number();
    test_acc_valid_name();
    test_acc_valid_amount();
    test_acc_valid_opening();
    test_auth_hash_and_pin_policy();
    test_auth_login();
    test_acc_change_pin();
    test_acc_create();
    test_txn_deposit();
    test_txn_withdraw();
    test_txn_balance_and_statements();
    test_txn_date_valid();
    test_txn_print_range();
    test_filestore_load_save();
    test_filestore_log_and_backup();

    printf("Unit tests: %d checks, %d failures\n", checks_run, checks_failed);
    if (chdir("/") != 0)
        die("chdir root");
    remove_tree(workdir);
    return checks_failed == 0 ? 0 : 1;
}
