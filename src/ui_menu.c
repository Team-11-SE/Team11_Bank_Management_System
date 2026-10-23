#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bank.h"

/* Reads a whole number. Returns 0 when the input ends (so the menu exits). */
static int read_int(const char *prompt)
{
    char line[LINE_LEN];
    char *end;
    long value;

    for (;;) {
        printf("%s", prompt);
        if (fgets(line, sizeof line, stdin) == NULL)
            return 0;
        value = strtol(line, &end, 10);
        while (isspace((unsigned char)*end))
            end++;
        if (end != line && *end == '\0' && value > -1000000000L && value < 1000000000L)
            return (int)value;
        printf("Please enter a whole number.\n");
    }
}

/* Reads an amount; keeps asking until it is valid. Returns -1 when the input ends. */
static double read_amount(const char *prompt)
{
    char line[LINE_LEN];
    char *end;
    double value;

    for (;;) {
        printf("%s", prompt);
        if (fgets(line, sizeof line, stdin) == NULL)
            return -1.0;
        value = strtod(line, &end);
        while (isspace((unsigned char)*end))
            end++;
        if (end != line && *end == '\0' && acc_valid_amount(value))
            return value;
        printf("Invalid amount. Enter a number greater than 0 with at most 2 decimals.\n");
    }
}

/* Reads a name; keeps asking until it is valid. Empty when the input ends. */
static void read_name(const char *prompt, char *out, int size)
{
    char line[LINE_LEN];
    size_t len;

    for (;;) {
        printf("%s", prompt);
        if (fgets(line, sizeof line, stdin) == NULL) {
            out[0] = '\0';
            return;
        }
        len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';
        if (acc_valid_name(line)) {
            snprintf(out, (size_t)size, "%s", line);
            return;
        }
        printf("Invalid name. Use letters and spaces only, at least 2 characters.\n");
    }
}

static void show_menu(void)
{
    printf("1. Create account\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Balance inquiry\n");
    printf("5. Mini statement (last 10 transactions)\n");
    printf("0. Exit\n");
}

void menu_run(void)
{
    struct account list[MAX_ACCOUNTS];
    int count = fs_load(list, MAX_ACCOUNTS);
    int choice;

    printf("Loaded %d account(s) from data/accounts.txt.\n\n", count);

    for (;;) {
        show_menu();
        choice = read_int("Enter choice: ");
        printf("\n");

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            char name[NAME_LEN];
            double opening;
            int new_no;

            read_name("Name: ", name, NAME_LEN);
            opening = read_amount("Opening balance: ");
            new_no = acc_create(list, &count, name, opening);
            if (new_no > 0) {
                printf("Account created. Your account number is %d.\n", new_no);
            } else if (new_no == -1) {
                printf("Invalid name. Use letters and spaces only, at least 2 characters.\n");
            } else if (new_no == -2) {
                printf("Invalid opening balance. It must be 0 or more with at most 2 decimals.\n");
            } else if (new_no == -3) {
                printf("Account list is full (%d accounts).\n", MAX_ACCOUNTS);
            } else {
                printf("Could not save the account to disk.\n");
            }
        } else if (choice == 2) {
            int acc_no = read_int("Account number: ");
            double amount = read_amount("Deposit amount: ");
            int rc = txn_deposit(list, count, acc_no, amount);
            int idx = acc_find(list, count, acc_no);

            if (rc == 0 && idx >= 0)
                printf("Deposited %.2f. New balance: %.2f\n", amount, list[idx].balance);
            else if (rc == 1)
                printf("Account %d not found.\n", acc_no);
            else
                printf("Deposit could not be completed (code %d).\n", rc);
        } else if (choice == 3) {
            int acc_no = read_int("Account number: ");
            double amount = read_amount("Withdrawal amount: ");
            int rc = txn_withdraw(list, count, acc_no, amount);
            int idx = acc_find(list, count, acc_no);

            if (rc == 0 && idx >= 0)
                printf("Withdrew %.2f. New balance: %.2f\n", amount, list[idx].balance);
            else if (rc == 1)
                printf("Account %d not found.\n", acc_no);
            else if (rc == 3 && idx >= 0)
                printf("Insufficient balance. Available: %.2f\n", list[idx].balance);
            else
                printf("Withdrawal could not be completed (code %d).\n", rc);
        } else if (choice == 4) {
            int acc_no = read_int("Account number: ");
            txn_balance(list, count, acc_no);
        } else if (choice == 5) {
            int acc_no = read_int("Account number: ");
            txn_print_last(acc_no, 10);
        } else {
            printf("Unknown option. Please choose between 0 and 5.\n");
        }
        printf("\n");
    }
}
