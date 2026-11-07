#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
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

/* Reads a secret without echo when stdin is a terminal (SAD 4.3.7). */
static void read_pin(const char *prompt, char *out, int size)
{
    struct termios oldt;
    struct termios newt;
    int tty = isatty(STDIN_FILENO);
    int echo_off = 0;
    size_t len;

    out[0] = '\0';
    printf("%s", prompt);
    fflush(stdout);

    if (tty && tcgetattr(STDIN_FILENO, &oldt) == 0) {
        newt = oldt;
        newt.c_lflag &= ~(tcflag_t)ECHO;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == 0)
            echo_off = 1;
    }

    if (fgets(out, size, stdin) == NULL)
        out[0] = '\0';

    if (echo_off) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        printf("\n");
    }

    len = strlen(out);
    while (len > 0 && (out[len - 1] == '\n' || out[len - 1] == '\r'))
        out[--len] = '\0';
}

static void show_main_menu(void)
{
    printf("1. Customer login\n");
    printf("2. Create account\n");
    printf("0. Exit\n");
}

static void show_customer_menu(void)
{
    printf("1. Deposit\n");
    printf("2. Withdraw\n");
    printf("3. Balance inquiry\n");
    printf("4. Mini statement (last 10 transactions)\n");
    printf("0. Logout\n");
}

/* Asks for the account number and PIN. Returns 1 when the login succeeded. */
static int customer_login(struct account *list, int count, int *idx_out)
{
    int acc_no = read_int("Account number: ");
    char pin[64];
    int rc;

    read_pin("PIN: ", pin, sizeof pin);
    rc = auth_login(list, count, acc_no, pin, idx_out);

    if (rc == 0) {
        printf("Login successful. Welcome, %s.\n\n", list[*idx_out].name);
    } else if (rc == 1) {
        printf("Account %d not found.\n\n", acc_no);
    } else {
        printf("Wrong PIN.\n\n");
    }
    return rc == 0;
}

static void create_account_flow(struct account *list, int *count)
{
    char name[NAME_LEN];
    char pin[64];
    char confirm[64];
    double opening;
    int new_no;

    read_name("Name: ", name, NAME_LEN);
    opening = read_amount("Opening balance: ");

    for (;;) {
        read_pin("New PIN (4 digits): ", pin, sizeof pin);
        if (!auth_valid_pin(pin)) {
            printf("PIN must be exactly 4 digits.\n");
            continue;
        }
        read_pin("Confirm PIN: ", confirm, sizeof confirm);
        if (strcmp(pin, confirm) != 0) {
            printf("PINs do not match. Try again.\n");
            continue;
        }
        break;
    }

    new_no = acc_create(list, count, name, opening, pin);
    if (new_no > 0) {
        printf("Account created. Your account number is %d.\n", new_no);
    } else if (new_no == -1) {
        printf("Invalid name. Use letters and spaces only, at least 2 characters.\n");
    } else if (new_no == -2) {
        printf("Invalid opening balance. It must be 0 or more with at most 2 decimals.\n");
    } else if (new_no == -3) {
        printf("Account list is full (%d accounts).\n", MAX_ACCOUNTS);
    } else if (new_no == -5) {
        printf("PIN must be exactly 4 digits.\n");
    } else {
        printf("Could not save the account to disk.\n");
    }
}

/* Runs the logged-in customer menu until logout (0). */
static void customer_session(struct account *list, int count, int idx)
{
    int acc_no = list[idx].acc_no;
    int choice;

    for (;;) {
        show_customer_menu();
        choice = read_int("Enter choice: ");
        printf("\n");

        if (choice == 0) {
            printf("Logged out.\n\n");
            return;
        } else if (choice == 1) {
            double amount = read_amount("Deposit amount: ");
            int rc = txn_deposit(list, count, acc_no, amount);

            if (rc == 0)
                printf("Deposited %.2f. New balance: %.2f\n", amount, list[idx].balance);
            else if (rc == 1)
                printf("Account %d not found.\n", acc_no);
            else
                printf("Deposit could not be completed (code %d).\n", rc);
        } else if (choice == 2) {
            double amount = read_amount("Withdrawal amount: ");
            int rc = txn_withdraw(list, count, acc_no, amount);

            if (rc == 0)
                printf("Withdrew %.2f. New balance: %.2f\n", amount, list[idx].balance);
            else if (rc == 1)
                printf("Account %d not found.\n", acc_no);
            else if (rc == 3)
                printf("Insufficient balance. Available: %.2f\n", list[idx].balance);
            else
                printf("Withdrawal could not be completed (code %d).\n", rc);
        } else if (choice == 3) {
            txn_balance(list, count, acc_no);
        } else if (choice == 4) {
            txn_print_last(acc_no, 10);
        } else {
            printf("Unknown option. Please choose between 0 and 4.\n");
        }
        printf("\n");
    }
}

void menu_run(void)
{
    static struct account list[MAX_ACCOUNTS];
    int count = fs_load(list, MAX_ACCOUNTS);
    int choice;

    printf("Loaded %d account(s) from data/accounts.txt.\n\n", count);

    for (;;) {
        show_main_menu();
        choice = read_int("Enter choice: ");
        printf("\n");

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            int idx = -1;

            if (customer_login(list, count, &idx))
                customer_session(list, count, idx);
        } else if (choice == 2) {
            create_account_flow(list, &count);
        } else {
            printf("Unknown option. Please choose between 0 and 2.\n");
        }
        printf("\n");
    }
}
