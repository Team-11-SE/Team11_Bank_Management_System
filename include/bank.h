
#ifndef BANK_H
#define BANK_H

#define MAX_ACCOUNTS 100
#define NAME_LEN 64
#define LINE_LEN 256
#define PIN_LEN 5
#define HASH_LEN 17
#define STATUS_LEN 8
#define MIN_BALANCE 500.00
#define DAILY_WITHDRAW_LIMIT 50000.00
#define DEPOSIT_LIMIT 200000.00
#define AMOUNT_CEILING 10000000.00
#define MAX_BACKUPS 10

struct account {
    int acc_no;
    char name[NAME_LEN];
    char pin_hash[HASH_LEN];
    double balance;
    char status[STATUS_LEN];
    int bad_tries;
};

/* ---- filestore.c ---- */
int fs_load(struct account *list, int max);
int fs_save(const struct account *list, int count);
int fs_log_txn(int acc_no, const char *type, double amount,
               double balance_after);
void fs_startup_backup(void);
void fs_check_transactions(void);

/* ---- accounts.c ---- */
int acc_find(const struct account *list, int count, int acc_no);
int acc_next_number(const struct account *list, int count);
int acc_valid_name(const char *name);
int acc_valid_amount(double amount);
int acc_valid_opening(double amount);

void auth_hash(const char *text, char out[HASH_LEN]);
int auth_valid_pin(const char *pin);
int auth_valid_pin_policy(const char *pin);

/* return codes: 0 = ok, 1 = account not found, 2 = invalid amount,
   3 = insufficient balance, 4 = save failed, 5 = daily limit would be crossed,
   6 = account not active, 7 = deposit limit exceeded */
int auth_login(struct account *list, int count, int acc_no,
               const char *pin, int *idx_out);

/* return codes: 0 = ok, 1 = wrong old PIN, 2 = weak new PIN,
   3 = save failed */
int acc_change_pin(struct account *list, int count, int idx,
                   const char *old_pin, const char *new_pin);

/* returns account number, or:
   -1 bad name, -2 bad opening balance, -3 account list full,
   -4 save failed, -5 bad PIN */
int acc_create(struct account *list, int *count, const char *name,
               double opening, const char *pin);

/* ---- transactions.c ---- */
/* return codes: 0 = ok, 1 = account not found, 2 = invalid amount,
   3 = insufficient balance, 4 = save failed, 6 = account not active,
   7 = deposit limit exceeded */
int txn_deposit(struct account *list, int count, int acc_no, double amount);
int txn_withdraw(struct account *list, int count, int acc_no, double amount);
void txn_balance(const struct account *list, int count, int acc_no);
void txn_print_last(int acc_no, int last_n);
double txn_withdrawn_today(int acc_no);
int txn_date_valid(const char *date);
void txn_print_range(int acc_no, const char *from, const char *to);

/* ---- ui_menu.c ---- */
void menu_run(void);

#endif
