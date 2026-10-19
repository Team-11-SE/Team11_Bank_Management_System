#include <stdio.h>
#include "bank.h"

/* Sprint 1 skeleton: implemented in tasks S1-05, S1-06 and S1-07. */

int txn_deposit(struct account *list, int count, int acc_no, double amount)
{
    (void)list; (void)count; (void)acc_no; (void)amount;
    return 4;
}

int txn_withdraw(struct account *list, int count, int acc_no, double amount)
{
    (void)list; (void)count; (void)acc_no; (void)amount;
    return 4;
}

void txn_balance(const struct account *list, int count, int acc_no)
{
    (void)list; (void)count; (void)acc_no;
    printf("Balance inquiry is not ready yet (task S1-07).\n");
}

void txn_print_last(int acc_no, int last_n)
{
    (void)acc_no; (void)last_n;
    printf("Mini statement is not ready yet (task S1-07).\n");
}
