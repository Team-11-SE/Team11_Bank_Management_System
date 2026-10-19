#ifndef BANK_H
#define BANK_H

#define MAX_ACCOUNTS 100
#define NAME_LEN 64
#define LINE_LEN 256

struct account {
    int acc_no;
    char name[NAME_LEN];
    double balance;
};

/* ---- filestore.c : all file access (data/accounts.txt, data/transactions.txt) ---- */
int fs_load(struct account *list, int max);                 /* returns number of accounts loaded */
int fs_save(const struct account *list, int count);         /* 0 = ok, 1 = file error */
int fs_log_txn(int acc_no, const char *type, double amount, double balance_after); /* 0 = ok, 1 = error */

/* ---- accounts.c : account list handling ---- */
int acc_find(const struct account *list, int count, int acc_no);    /* index, or -1 if not found */
int acc_next_number(const struct account *list, int count);         /* 1001 for the first account, then highest + 1 */
int acc_valid_name(const char *name);                               /* 1 = ok, 0 = bad */
int acc_valid_amount(double amount);                                /* 1 = ok, 0 = bad (must be > 0, max 2 decimals) */
/* returns the new account number, or:
   -1 bad name, -2 bad opening balance, -3 account list full, -4 save failed */
int acc_create(struct account *list, int *count, const char *name, double opening);

/* ---- transactions.c : money operations ---- */
/* return codes: 0 = ok, 1 = account not found, 2 = invalid amount,
   3 = insufficient balance, 4 = save failed */
int  txn_deposit(struct account *list, int count, int acc_no, double amount);
int  txn_withdraw(struct account *list, int count, int acc_no, double amount);
void txn_balance(const struct account *list, int count, int acc_no);   /* read-only, prints */
void txn_print_last(int acc_no, int last_n);                           /* prints, newest first */

/* ---- ui_menu.c : console menu ---- */
void menu_run(void);

#endif
