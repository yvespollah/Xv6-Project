// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/file.h"
#include "user/user.h"


char *argv[] = { "sh", 0 };  // Shell arguments

int main(void) {
    int pid;

    if (open("console", O_RDWR) < 0) {
        mknod("console", CONSOLE, 0);
        open("console", O_RDWR);
    }
    dup(0);  // stdout
    dup(0);  // stderr

    for (;;) {
        printf("\n\t\t====================================================\n");
        printf("\n\t\t***** Welcome to our new version of XV6 *****\n\n\n");
        printf("\t\t\t Designed by M1 students of UY1\n");
        printf("\t\t===================================================\n");
        printf("\n* Please tell us what you want to do *\n");
        printf("1. List available programs\n");
        printf("2. Execute a shell\n");
        printf("3. Quit XV6\n");
        printf("\n====================================================\n");
        printf("Select an option: --> ");

        char input[10] = {0};  // Ensure the buffer is initialized
        gets(input, sizeof(input));  // Get user input safely

        if (input[0] == '1') {
            // Option 1: List available programs (using `ls`)
            printf("Listing available programs...\n");
            pid = fork();
            if (pid < 0) {
                printf("init: fork failed\n");
                exit(1);
            }
            if (pid == 0) {
                exec("ls", argv);
                printf("init: exec ls failed\n");
                exit(1);
            }
            wait(0);  // Wait for `ls` to finish
        } 
        else if (input[0] == '2') {
            // Option 2: Start the shell
            printf("\nEnter exit to go back\nStarting the shell...\n");
            printf("\n\n\n\n\n\n\n\n\n\n");
                        pid = fork();
            if (pid < 0) {
                printf("init: fork failed\n");
                exit(1);
            }
            if (pid == 0) {
                exec("sh", argv);
                printf("init: exec sh failed\n");
                exit(1);
            }
            wait(0);  // Wait for the shell to exit
        } 
        else if (input[0] == '3') {
            // Option 3: Quit xv6
            printf("Shutting down XV6...\n");
            printf("Enter Ctrl+A, release the key and press X to exit the emulator\n");
            sleep(3);
            exit(0);
        } 
        else {
            // Invalid input
            printf("Invalid option. Please enter 1, 2, or 3.\n");
        }
    }
}
