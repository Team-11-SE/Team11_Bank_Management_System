#include <stdio.h>
#include <string.h>
#include "bank.h"
#include <time.h>

/* Return codes: 0 = ok, 1 = account not found, 2 = invalid amount,
   3 = insufficient balance, 4 = save failed. */

/* Writes the log line first, then saves the new balance (BMS-F-008). */
static int save_and_log(struct account *list, int count, int idx,
                        const char *type, double amount)
{
    if (fs_log_txn(list[idx].acc_no, type, amount, list[idx].balance) != 0)
        return 4;
    if (fs_save(list, count) != 0)
        return 4;
    return 0;
}

int txn_deposit(struct account *list, int count, int acc_no, double amount)
{
    int idx = acc_find(list, count, acc_no);

    if (idx < 0)
        return 1;
    if (!acc_valid_amount(amount))
        return 2;
    if (strcmp(list[idx].status, "ACTIVE") != 0)
        return 6;
    if (amount > DEPOSIT_LIMIT)
        return 7;

    list[idx].balance += amount;
    return save_and_log(list, count, idx, "DEPOSIT", amount);
}

int txn_withdraw(struct account *list, int count, int acc_no, double amount)
{
    int idx = acc_find(list, count, acc_no);

    if (idx < 0)
        return 1;
    if (!acc_valid_amount(amount))
        return 2;
    if (strcmp(list[idx].status, "ACTIVE") != 0)
        return 6;
    if (amount > list[idx].balance - MIN_BALANCE)
        return 3;
    if (txn_withdrawn_today(acc_no) + amount > DAILY_WITHDRAW_LIMIT)
        return 5;

    list[idx].balance -= amount;
    return save_and_log(list, count, idx, "WITHDRAW", amount);
}

void txn_balance(const struct account *list, int count, int acc_no)
{
    int idx = acc_find(list, count, acc_no);

    if (idx < 0) {
        printf("Account %d not found.\n", acc_no);
        return;
    }

    printf("Balance for account %d (%s): %.2f\n",
           acc_no, list[idx].name, list[idx].balance);
}

void txn_print_last(int acc_no, int last_n)
{
    FILE *fp = fopen("data/transactions.txt", "r");
    char line[LINE_LEN];
    char keep[10][LINE_LEN];
    int total = 0;
    int i;

    if (last_n > 10)
        last_n = 10;
    if (last_n < 1)
        last_n = 1;

    if (fp == NULL) {
        printf("No transactions found for account %d.\n", acc_no);
        return;
    }

    while (fgets(line, sizeof line, fp) != NULL) {
        int acc;

        if (sscanf(line, "%*d,%d,", &acc) == 1 && acc == acc_no) {
            snprintf(keep[total % 10], LINE_LEN, "%s", line);
            total++;
        }
    }

    fclose(fp);

    if (total == 0) {
        printf("No transactions found for account %d.\n", acc_no);
        return;
    }

    printf("--- Last %d transaction(s) for account %d, newest first ---\n",
           total < last_n ? total : last_n, acc_no);

    for (i = 0; i < last_n && i < total; i++) {
        char *l = keep[(total - 1 - i) % 10];
        l[strcspn(l, "\n")] = '\0';
        printf("%s\n", l);
    }
}
/* Sum of today's WITHDRAW lines for one account (BMS-F-007). */
double txn_withdrawn_today(int acc_no)
{
    FILE *fp = fopen("data/transactions.txt", "r");
    char line[LINE_LEN];
    char when[32];
    char today[11];
    time_t now;
    struct tm *tm_info;
    double sum = 0.0;

    if (fp == NULL)
        return 0.0;

    now = time(NULL);
    tm_info = localtime(&now);
    strftime(today, sizeof today, "%Y-%m-%d", tm_info);

    while (fgets(line, sizeof line, fp) != NULL) {
        int acc;
        char type[16];
        double amount;

        if (sscanf(line, "%*d,%d,%15[^,],%lf,%*[^,],%31[^\n]",
                   &acc, type, &amount, when) == 4
            && acc == acc_no && strcmp(type, "WITHDRAW") == 0
            && strncmp(when, today, 10) == 0)
            sum += amount;
    }
    fclose(fp);
    return sum;
}

int txn_date_valid(const char *date)
{
    int y;
    int m;
    int d;
    char extra;

    if (strlen(date) != 10)
        return 0;
    if (sscanf(date, "%4d-%2d-%2d%c", &y, &m, &d, &extra) != 3)
        return 0;
    if (y < 2000 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31)
        return 0;
    return 1;
}

void txn_print_range(int acc_no, const char *from, const char *to)
{
    FILE *fp = fopen("data/transactions.txt", "r");
    char line[LINE_LEN];
    int count = 0;

    if (fp == NULL) {
        printf("No transactions in this period\n");
        return;
    }

    printf("--- Statement for account %d from %s to %s ---\n", acc_no, from, to);
    while (fgets(line, sizeof line, fp) != NULL) {
        int id;
        int acc;
        char type[16];
        double amount;
        double balance;
        char when[32];

        if (sscanf(line, "%d,%d,%15[^,],%lf,%lf,%31[^\n]",
                   &id, &acc, type, &amount, &balance, when) == 6
            && acc == acc_no
            && strncmp(when, from, 10) >= 0
            && strncmp(when, to, 10) <= 0) {
            line[strcspn(line, "\n")] = '\0';
            printf("%s\n", line);
            count++;
        }
    }
    fclose(fp);

    if (count == 0)
        printf("No transactions in this period\n");
    else
        printf("%d transaction(s) in this period.\n", count);
}

