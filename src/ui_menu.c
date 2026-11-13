
#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "bank.h"

/* Hash of the admin password "admin1234"; the plain password is never stored. */
#define ADMIN_HASH "c2109c4aa23d0ff6"

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
        printf("Invalid amount. Enter a number greater than 0 and below Rs. 10000000.00 with at most 2 decimals.\n");
    }
}

/* Reads a date in YYYY-MM-DD form. */
static void read_date(const char *prompt, char *out, int size)
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
        if (txn_date_valid(line)) {
            snprintf(out, (size_t)size, "%s", line);
            return;
        }
        printf("Invalid date. Use YYYY-MM-DD.\n");
    }
}


/* Reads an opening balance: 0 is allowed for a new account (BMS-F-019). */
static double read_opening(const char *prompt)
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
        if (end != line && *end == '\0' && acc_valid_opening(value))
            return value;
        printf("Invalid opening balance. Enter 0 or more (below Rs. 10000000.00) with at most 2 decimals.\n");
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
    printf("3. Admin login\n");
    printf("0. Exit\n");
}

static void show_customer_menu(void)
{
    printf("1. Deposit\n");
    printf("2. Withdraw\n");
    printf("3. Balance inquiry\n");
    printf("4. Mini statement (last 10 transactions)\n");
    printf("5. Change PIN\n");
    printf("6. Statement between two dates\n");
    printf("0. Logout\n");
}

/* Asks for the account number and PIN. Returns 1 when the login succeeded. */
static int customer_login(struct account *list, int count, int *idx_out)
{
    int acc_no = read_int("Account number: ");
    char pin[64];
    int rc;

    *idx_out = -1;
    read_pin("PIN: ", pin, sizeof pin);
    if (!auth_valid_pin(pin)) {
        printf("PIN must be exactly 4 digits.\n\n");
        return 0;
    }

    rc = auth_login(list, count, acc_no, pin, idx_out);
    if (rc == 0) {
        printf("Login successful. Welcome, %s.\n\n", list[*idx_out].name);
    } else if (rc == 1) {
        printf("Account %d not found.\n\n", acc_no);
    } else if (rc == 2) {
        if (*idx_out >= 0 && strcmp(list[*idx_out].status, "LOCKED") == 0)
            printf("Wrong PIN. Account is LOCKED after 3 failed attempts.\n\n");
        else
            printf("Wrong PIN. Attempt %d of 3.\n\n", list[*idx_out].bad_tries);
    } else if (rc == 3) {
        printf("Account is LOCKED. Contact the bank admin.\n\n");
    } else if (rc == 4) {
        printf("Account is CLOSED.\n\n");
    } else {
        printf("Could not save the failed attempt.\n\n");
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
    if (name[0] == '\0')
        return;                         /* EOF: do not hang in the PIN loop */
    opening = read_opening("Opening balance: ");
    if (opening < 0.0)
        return;                         /* EOF */

    for (;;) {
        read_pin("New PIN (4 digits): ", pin, sizeof pin);
        if (pin[0] == '\0')
            return;                     /* EOF */
        if (!auth_valid_pin(pin)) {
            printf("PIN must be exactly 4 digits.\n");
            continue;
        }
        if (!auth_valid_pin_policy(pin)) {
            printf("Weak PIN. Do not use 0000, 1111, 1234 or 4321.\n");
            continue;
        }
        read_pin("Confirm PIN: ", confirm, sizeof confirm);
        if (confirm[0] == '\0')
            return;                     /* EOF */
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

/* Changes the PIN after checking the old PIN and the weak-PIN policy (S2-03). */
static void change_pin_flow(struct account *list, int count, int idx)
{
    char old_pin[64];
    char new_pin[64];
    char confirm[64];
    int rc;

    read_pin("Old PIN: ", old_pin, sizeof old_pin);
    if (!auth_valid_pin(old_pin)) {
        printf("PIN must be exactly 4 digits.\n");
        return;
    }

    read_pin("New PIN (4 digits): ", new_pin, sizeof new_pin);
    if (!auth_valid_pin(new_pin)) {
        printf("PIN must be exactly 4 digits.\n");
        return;
    }
    if (!auth_valid_pin_policy(new_pin)) {
        printf("Weak PIN. Do not use 0000, 1111, 1234 or 4321.\n");
        return;
    }

    read_pin("Confirm new PIN: ", confirm, sizeof confirm);
    if (strcmp(new_pin, confirm) != 0) {
        printf("PINs do not match. PIN not changed.\n");
        return;
    }

    rc = acc_change_pin(list, count, idx, old_pin, new_pin);
    if (rc == 0)
        printf("PIN changed.\n");
    else if (rc == 1)
        printf("Wrong old PIN. PIN not changed.\n");
    else if (rc == 2)
        printf("Weak PIN. Do not use 0000, 1111, 1234 or 4321.\n");
    else
        printf("Could not save the new PIN.\n");
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
            else if (rc == 6)
                printf("Account is not active.\n");
            else if (rc == 7)
                printf("Deposit limit is Rs. 200000.00 per deposit.\n");
            else
                printf("Deposit could not be completed (code %d).\n", rc);
        } else if (choice == 2) {
            double amount = read_amount("Withdrawal amount: ");
            int rc = txn_withdraw(list, count, acc_no, amount);

            if (rc == 0)
                printf("Withdrew %.2f. New balance: %.2f\n", amount, list[idx].balance);
            else if (rc == 1)
                printf("Account %d not found.\n", acc_no);
            else if (rc == 3) {
                double available = list[idx].balance - MIN_BALANCE;

                if (available < 0.0)
                    available = 0.0;
                printf("Insufficient balance. Minimum balance Rs. 500.00 must remain. Available: %.2f\n",
                       available);
            } else if (rc == 5) {
                printf("Daily withdrawal limit Rs. 50000.00 reached. Remaining today: %.2f\n",
                       DAILY_WITHDRAW_LIMIT - txn_withdrawn_today(acc_no));
            } else if (rc == 6) {
                printf("Account is not active.\n");
            } else {
                printf("Withdrawal could not be completed (code %d).\n", rc);
            }
        } else if (choice == 3) {
            txn_balance(list, count, acc_no);
        } else if (choice == 4) {
            txn_print_last(acc_no, 10);
        } else if (choice == 5) {
            change_pin_flow(list, count, idx);
            } else if (choice == 6) {
    char from[16];
    char to[16];

    read_date("From date (YYYY-MM-DD): ", from, sizeof from);
    read_date("To date (YYYY-MM-DD): ", to, sizeof to);
    txn_print_range(acc_no, from, to);
        } else {
            printf("Unknown option. Please choose between 0 and 6.\n");
        }
        printf("\n");
    }
}

static void show_admin_menu(void)
{
    printf("1. List all accounts and total money\n");
    printf("2. Show one account\n");
    printf("3. Unlock a locked account\n");
    printf("4. Close a zero-balance account\n");
    printf("0. Exit admin\n");
}

static void admin_list(struct account *list, int count)
{
    double total = 0.0;
    int i;

    printf("--- All accounts ---\n");
    for (i = 0; i < count; i++) {
        printf("%d | %s | %s | %.2f\n", list[i].acc_no, list[i].name,
               list[i].status, list[i].balance);
        total += list[i].balance;
    }
    printf("Total money in the bank: %.2f\n", total);
}

static void admin_show(struct account *list, int count)
{
    int acc_no = read_int("Account number: ");
    int idx = acc_find(list, count, acc_no);

    if (idx < 0) {
        printf("Account %d not found.\n", acc_no);
        return;
    }
    printf("Number: %d\n", list[idx].acc_no);
    printf("Name: %s\n", list[idx].name);
    printf("Status: %s\n", list[idx].status);
    printf("Balance: %.2f\n", list[idx].balance);
}

static void admin_unlock(struct account *list, int count)
{
    int acc_no = read_int("Account number: ");
    int idx = acc_find(list, count, acc_no);

    if (idx < 0) {
        printf("Account %d not found.\n", acc_no);
        return;
    }
    if (strcmp(list[idx].status, "LOCKED") != 0) {
        printf("Account %d is %s, not LOCKED.\n", acc_no, list[idx].status);
        return;
    }
    snprintf(list[idx].status, STATUS_LEN, "ACTIVE");
    list[idx].bad_tries = 0;
    if (fs_save(list, count) != 0) {
        printf("Could not save the unlock.\n");
        return;
    }
    printf("Account %d unlocked.\n", acc_no);
}

static void admin_close(struct account *list, int count)
{
    int acc_no = read_int("Account number: ");
    int idx = acc_find(list, count, acc_no);

    if (idx < 0) {
        printf("Account %d not found.\n", acc_no);
        return;
    }
    if (strcmp(list[idx].status, "CLOSED") == 0) {
        printf("Account %d is already CLOSED.\n", acc_no);
        return;
    }
    if (list[idx].balance != 0.0) {
        printf("Account %d has a balance of %.2f. Only a zero balance can be closed.\n",
               acc_no, list[idx].balance);
        return;
    }
    snprintf(list[idx].status, STATUS_LEN, "CLOSED");
    if (fs_save(list, count) != 0) {
        printf("Could not save the closure.\n");
        return;
    }
    printf("Account %d closed.\n", acc_no);
}

/* Admin session: opened only by the admin password (PRJ-SR-006). */
static void admin_session(struct account *list, int count)
{
    int choice;

    for (;;) {
        show_admin_menu();
        choice = read_int("Enter choice: ");
        printf("\n");

        if (choice == 0) {
            printf("Admin session closed.\n\n");
            return;
        } else if (choice == 1) {
            admin_list(list, count);
        } else if (choice == 2) {
            admin_show(list, count);
        } else if (choice == 3) {
            admin_unlock(list, count);
        } else if (choice == 4) {
            admin_close(list, count);
        } else {
            printf("Unknown option. Please choose between 0 and 4.\n");
        }
        printf("\n");
    }
}

/* Returns 1 after a correct admin password. */
static int admin_login(void)
{
    char password[64];
    char hash[HASH_LEN];

    read_pin("Admin password: ", password, sizeof password);
    auth_hash(password, hash);
    if (strcmp(hash, ADMIN_HASH) != 0) {
        printf("Wrong admin password.\n\n");
        return 0;
    }
    printf("Admin login successful.\n\n");
    return 1;
}

void menu_run(void)
{
    static struct account list[MAX_ACCOUNTS];
    int count;
    int choice;

    fs_startup_backup();
    count = fs_load(list, MAX_ACCOUNTS);
    fs_check_transactions();

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
        } else if (choice == 3) {
            if (admin_login())
                admin_session(list, count);
        } else {
            printf("Unknown option. Please choose between 0 and 3.\n");
        }
        printf("\n");
    }
}
