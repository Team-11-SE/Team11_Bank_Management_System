
#ifndef BANK_H
#define BANK_H

#define MAX_ACCOUNTS 100
#define NAME_LEN 64
#define LINE_LEN 256
#define PIN_LEN 5
#define HASH_LEN 17
#define STATUS_LEN 8

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

/* ---- accounts.c ---- */
int acc_find(const struct account *list, int count, int acc_no);
int acc_next_number(const struct account *list, int count);
int acc_valid_name(const char *name);
int acc_valid_amount(double amount);

void auth_hash(const char *text, char out[HASH_LEN]);
int auth_valid_pin(const char *pin);
int auth_valid_pin_policy(const char *pin);

/* return codes: 0 = ok, 1 = account not found, 2 = wrong PIN (counted),
   3 = account LOCKED, 4 = account CLOSED, 5 = could not save failed attempt */
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
   3 = insufficient balance, 4 = save failed, 6 = account not active */
int txn_deposit(struct account *list, int count, int acc_no, double amount);
int txn_withdraw(struct account *list, int count, int acc_no, double amount);
void txn_balance(const struct account *list, int count, int acc_no);
void txn_print_last(int acc_no, int last_n);

/* ---- ui_menu.c ---- */
void menu_run(void);

#endif
