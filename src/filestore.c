#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "bank.h"

#define ACC_FILE "data/accounts.txt"
#define TXN_FILE "data/transactions.txt"
#define BACKUP_DIR "data/backup"
#define PATH_LEN 512

static int name_cmp(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

static void copy_file(const char *src, const char *dst)
{
    FILE *in = fopen(src, "rb");
    FILE *out;
    char buf[4096];
    size_t n;

    if (in == NULL)
        return;                         /* nothing to back up yet */
    out = fopen(dst, "wb");
    if (out == NULL) {
        fclose(in);
        printf("Warning: backup not created.\n");
        return;
    }
    while ((n = fread(buf, 1, sizeof buf, in)) > 0)
        fwrite(buf, 1, n, out);
    fclose(in);
    fclose(out);
}

/* Keep only the newest MAX_BACKUPS files that start with prefix. */
static void rotate_backups(const char *prefix)
{
    DIR *dir = opendir(BACKUP_DIR);
    struct dirent *ent;
    char names[128][64];
    int n = 0;
    int i;
    size_t plen = strlen(prefix);

    if (dir == NULL)
        return;
    while ((ent = readdir(dir)) != NULL && n < 128) {
        if (strncmp(ent->d_name, prefix, plen) == 0
            && strstr(ent->d_name, ".txt") != NULL)
            snprintf(names[n++], sizeof names[0], "%s", ent->d_name);
    }
    closedir(dir);

    qsort(names, (size_t)n, sizeof names[0], name_cmp);
    for (i = 0; i < n - MAX_BACKUPS; i++) {
        char path[PATH_LEN];
        snprintf(path, sizeof path, "%s/%s", BACKUP_DIR, names[i]);
        remove(path);
    }
}

void fs_startup_backup(void)
{
    char stamp[32];
    char dst[PATH_LEN];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    if (mkdir(BACKUP_DIR, 0755) != 0 && errno != EEXIST) {
        printf("Warning: backup not created.\n");
        return;
    }

    strftime(stamp, sizeof stamp, "%Y-%m-%d_%H%M", tm_info);

    snprintf(dst, sizeof dst, "%s/accounts_%s.txt", BACKUP_DIR, stamp);
    copy_file(ACC_FILE, dst);
    snprintf(dst, sizeof dst, "%s/transactions_%s.txt", BACKUP_DIR, stamp);
    copy_file(TXN_FILE, dst);

    rotate_backups("accounts_");
    rotate_backups("transactions_");
}

void fs_check_transactions(void)
{
    FILE *fp = fopen(TXN_FILE, "r");
    char line[LINE_LEN];
    int lineno = 0;
    int good = 0;

    if (fp == NULL)
        return;

    while (fgets(line, sizeof line, fp) != NULL) {
        int id;
        int acc;
        char type[16];
        double amount;
        double balance;
        lineno++;

        if (sscanf(line, "%d,%d,%15[^,],%lf,%lf,%*31[^\n]",
                   &id, &acc, type, &amount, &balance) == 5
            && id > 0 && acc > 0
            && (strcmp(type, "DEPOSIT") == 0 || strcmp(type, "WITHDRAW") == 0)
            && amount > 0.0 && balance >= 0.0)
            good++;
        else
            printf("Warning: data/transactions.txt line %d is damaged and was skipped.\n", lineno);
    }
    fclose(fp);

    if (good > 0)
        printf("Checked %d transaction line(s).\n", good);
}

int fs_load(struct account *list, int max)
{
    FILE *fp = fopen(ACC_FILE, "r");
    char line[LINE_LEN];
    int count = 0;
    int lineno = 0;

    if (fp == NULL)
        return 0;                       /* no file yet = empty bank */

    while (count < max && fgets(line, sizeof line, fp) != NULL) {
        struct account tmp;
        lineno++;

        if (sscanf(line, "%d,%63[^,],%16[^,],%lf,%7[^,],%d",
                   &tmp.acc_no, tmp.name, tmp.pin_hash, &tmp.balance,
                   tmp.status, &tmp.bad_tries) == 6
            && tmp.acc_no > 0 && strlen(tmp.pin_hash) == 16
            && strlen(tmp.status) > 0) {
            list[count] = tmp;
            count++;
        } else {
            /* damaged record: warn and keep the other accounts (BMS-F-017) */
            printf("Warning: data/accounts.txt line %d is damaged and was skipped.\n", lineno);
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
        fprintf(fp, "%d,%s,%s,%.2f,%s,%d\n", list[i].acc_no, list[i].name,
                list[i].pin_hash, list[i].balance, list[i].status,
                list[i].bad_tries);
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
