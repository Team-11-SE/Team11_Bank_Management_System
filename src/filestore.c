#include "bank.h"

/* Sprint 1 skeleton: implemented in task S1-02. */

int fs_load(struct account *list, int max)
{
    (void)list; (void)max;
    return 0;
}

int fs_save(const struct account *list, int count)
{
    (void)list; (void)count;
    return 1;
}

int fs_log_txn(int acc_no, const char *type, double amount, double balance_after)
{
    (void)acc_no; (void)type; (void)amount; (void)balance_after;
    return 1;
}
