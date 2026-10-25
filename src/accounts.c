#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "bank.h"

int acc_find(const struct account *list, int count, int acc_no)
{
    int i;
    for (i = 0; i < count; i++) {
        if (list[i].acc_no == acc_no)
            return i;
    }
    return -1;
}

int acc_next_number(const struct account *list, int count)
{
    int i;
    int highest = 1000;                 /* first account becomes 1001 */

    for (i = 0; i < count; i++) {
        if (list[i].acc_no > highest)
            highest = list[i].acc_no;
    }
    return highest + 1;
}

int acc_valid_name(const char *name)
{
    size_t i;
    size_t len = strlen(name);
    int letters = 0;

    if (len < 2 || len >= NAME_LEN)
        return 0;
    if (!isalpha((unsigned char)name[0]) || !isalpha((unsigned char)name[len - 1]))
        return 0;                        /* no leading or trailing space */

    for (i = 0; i < len; i++) {
        if (isalpha((unsigned char)name[i])) {
            letters++;
        } else if (name[i] == ' ') {
            if (name[i - 1] == ' ' || name[i + 1] == ' ')
                return 0;                /* no double spaces */
        } else {
            return 0;
        }
    }
    return letters >= 2;
}

int acc_valid_amount(double amount)
{
    double cents;
    long long whole;
    double diff;

    if (amount <= 0.0 || amount > 100000000.0)
        return 0;

    cents = amount * 100.0;
    whole = (long long)(cents + 0.5);
    diff = cents - (double)whole;
    return diff < 0.000001 && diff > -0.000001;   /* at most 2 decimal places */
}

int acc_create(struct account *list, int *count, const char *name, double opening)
{
    int new_no;

    if (!acc_valid_name(name))
        return -1;
    if (!(opening == 0.0 || acc_valid_amount(opening)))
        return -2;
    if (*count >= MAX_ACCOUNTS)
        return -3;

    new_no = acc_next_number(list, *count);
    list[*count].acc_no = new_no;
    snprintf(list[*count].name, NAME_LEN, "%s", name);
    list[*count].balance = opening;
    (*count)++;

    if (fs_save(list, *count) != 0)
        return -4;
    return new_no;
}
