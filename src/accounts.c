
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
    int highest = 1000;

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
    if (!isalpha((unsigned char)name[0]) ||
        !isalpha((unsigned char)name[len - 1]))
        return 0;

    for (i = 0; i < len; i++) {
        if (isalpha((unsigned char)name[i])) {
            letters++;
        } else if (name[i] == ' ') {
            if (name[i - 1] == ' ' || name[i + 1] == ' ')
                return 0;
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

    if (amount <= 0.0 || amount >= AMOUNT_CEILING)
        return 0;   /* PRJ-SR-007 */

    cents = amount * 100.0;
    whole = (long long)(cents + 0.5);
    diff = cents - (double)whole;
    return diff < 0.000001 && diff > -0.000001;
}

int acc_valid_opening(double amount)
{
    if (amount == 0.0)
        return 1;
    return acc_valid_amount(amount);
}

/* FNV-1a: only the hash is stored. */
void auth_hash(const char *text, char out[HASH_LEN])
{
    unsigned long long h = 1469598103934665603ULL;
    const unsigned char *p = (const unsigned char *)text;

    while (*p != '\0') {
        h ^= (unsigned long long)*p++;
        h *= 1099511628211ULL;
    }
    snprintf(out, HASH_LEN, "%016llx", h);
}

int auth_valid_pin(const char *pin)
{
    size_t i;

    if (strlen(pin) != 4)
        return 0;

    for (i = 0; i < 4; i++) {
        if (!isdigit((unsigned char)pin[i]))
            return 0;
    }
    return 1;
}

/* 1 = valid PIN; 0 = invalid or weak PIN. */
int auth_valid_pin_policy(const char *pin)
{
    if (!auth_valid_pin(pin))
        return 0;

    if (pin[0] == pin[1] &&
        pin[1] == pin[2] &&
        pin[2] == pin[3])
        return 0;

    if (strcmp(pin, "1234") == 0 ||
        strcmp(pin, "4321") == 0)
        return 0;

    return 1;
}

int auth_login(struct account *list, int count, int acc_no,
               const char *pin, int *idx_out)
{
    int idx = acc_find(list, count, acc_no);
    char hash[HASH_LEN];

    if (idx < 0)
        return 1;
    if (strcmp(list[idx].status, "LOCKED") == 0)
        return 3;
    if (strcmp(list[idx].status, "CLOSED") == 0)
        return 4;
    if (!auth_valid_pin(pin))
        return 2;

    auth_hash(pin, hash);
    if (strcmp(hash, list[idx].pin_hash) != 0) {
        list[idx].bad_tries++;
        if (list[idx].bad_tries >= 3)
            snprintf(list[idx].status, STATUS_LEN, "LOCKED");

        *idx_out = idx;
        if (fs_save(list, count) != 0)
            return 5;
        return 2;
    }

    *idx_out = idx;
    return 0;
}

/* Changes a PIN after verifying the old PIN. */
int acc_change_pin(struct account *list, int count, int idx,
                   const char *old_pin, const char *new_pin)
{
    char hash[HASH_LEN];

    if (idx < 0 || idx >= count)
        return 1;

    if (!auth_valid_pin(old_pin))
        return 1;

    auth_hash(old_pin, hash);
    if (strcmp(hash, list[idx].pin_hash) != 0)
        return 1;

    if (!auth_valid_pin_policy(new_pin))
        return 2;

    auth_hash(new_pin, list[idx].pin_hash);

    if (fs_save(list, count) != 0)
        return 3;

    return 0;
}

int acc_create(struct account *list, int *count, const char *name,
               double opening, const char *pin)
{
    int new_no;

    if (!acc_valid_name(name))
        return -1;
    if (!(opening == 0.0 || acc_valid_amount(opening)))
        return -2;
    if (!auth_valid_pin(pin))
        return -5;
    if (*count >= MAX_ACCOUNTS)
        return -3;

    new_no = acc_next_number(list, *count);
    list[*count].acc_no = new_no;
    snprintf(list[*count].name, NAME_LEN, "%s", name);
    auth_hash(pin, list[*count].pin_hash);
    list[*count].balance = opening;
    snprintf(list[*count].status, STATUS_LEN, "ACTIVE");
    list[*count].bad_tries = 0;
    (*count)++;

    if (fs_save(list, *count) != 0)
        return -4;

    return new_no;
}
