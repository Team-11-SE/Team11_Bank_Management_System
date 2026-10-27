#include <stdio.h>
#include "bank.h"

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

    if (amount > list[idx].balance)
        return 3;

    list[idx].balance -= amount;
    return save_and_log(list, count, idx, "WITHDRAW", amount);
}

void txn_balance(const struct account *list, int count, int acc_no)
{
    (void)list;
    (void)count;
    (void)acc_no;
    printf("Balance inquiry is not ready yet (task S1-07).\n");
}

void txn_print_last(int acc_no, int last_n)
{
    (void)acc_no;
    (void)last_n;
    printf("Mini statement is not ready yet (task S1-07).\n");
}
