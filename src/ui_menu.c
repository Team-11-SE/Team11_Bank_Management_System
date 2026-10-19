#include <stdio.h>
#include "bank.h"

/* Sprint 1 skeleton. The real menu with safe input handling is task S1-03. */
void menu_run(void)
{
    int choice = -1;

    while (choice != 0) {
        printf("1. Create account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Balance inquiry\n");
        printf("5. Mini statement\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Please enter a number.\n");
            return;
        }
        if (choice != 0)
            printf("[menu skeleton] Option %d is not ready yet (task S1-03).\n\n", choice);
    }
}
