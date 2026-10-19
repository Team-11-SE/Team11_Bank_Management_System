#include <stdio.h>
#include <string.h>
#include <time.h>
#include "bank.h"

#define ACC_FILE "data/accounts.txt"
#define TXN_FILE "data/transactions.txt"

int fs_load(struct account *list, int max)
{
    FILE *fp = fopen(ACC_FILE, "r");
    char line[LINE_LEN];
    int count = 0;

    if (fp == NULL)
        return 0;                       /* no file yet = empty bank */

    while (count < max && fgets(line, sizeof line, fp) != NULL) {
        struct account tmp;
        if (sscanf(line, "%d,%63[^,],%lf", &tmp.acc_no, tmp.name, &tmp.balance) == 3) {
            list[count] = tmp;
            count++;
        }
    }
    fclose(fp);
    return count;
}

int fs_save(const struct account *list, int count)
{
    FILE *fp = fopen(ACC_FILE, "w");
    int i;

    if (fp == NULL)
        return 1;
    for (i = 0; i < count; i++)
        fprintf(fp, "%d,%s,%.2f\n", list[i].acc_no, list[i].name, list[i].balance);
    fclose(fp);
    return 0;
}

int fs_log_txn(int acc_no, const char *type, double amount, double balance_after)
{
    FILE *fp;
    char when[32];
    time_t now;
    struct tm *tm_info;
    int id = 1;

    /* transaction id = number of lines already in the log + 1 */
    fp = fopen(TXN_FILE, "r");
    if (fp != NULL) {
        int ch;
        while ((ch = fgetc(fp)) != EOF)
            if (ch == '\n')
                id++;
        fclose(fp);
    }

    fp = fopen(TXN_FILE, "a");
    if (fp == NULL)
        return 1;

    now = time(NULL);
    tm_info = localtime(&now);
    strftime(when, sizeof when, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(fp, "%d,%d,%s,%.2f,%.2f,%s\n", id, acc_no, type, amount, balance_after, when);
    fclose(fp);
    return 0;
}
