// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// void loginf(){
//   char *argv[] = { "login", 0 };  // Arguments to pass to login (if needed)
//     exec("login", argv);  // This will replace the init process with the login program

//     // If exec fails, you can add error handling here
//     printf("exec login failed\n");
//     exit(0);
// }

 char *argv[] = { "sh", 0 };


int
main(void)
{

  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  for(;;){
    clear();
    printf("\t\t====================================================\n");
    printf("\n\t\t***** Welcome to our new version of xv6 *****\n\n\n");
    printf("\t\t\t designed by M1 students of UY1\n");
    printf("\t\t===================================================\n");
    printf("\n* Please tell us what you want to do *\n");
    printf("1. List system calls\n");
    printf("2. execute a system call");
    printf("\n\n\t\t Quit XV6 (Ctrl+A + X)\t\t\n\n");
    printf("=====================\n\n");
    printf("Select an option:===--> ");

    char input[10];
    char *argv[] = { "sh", 0 }; // Arguments for shell

    gets(input, sizeof(input));  // Get the user input
    if (input[0] == '1') 
    {
        // Option 1: List system calls (using existing `ls` functionality)
        printf("Listing system calls...\n");
         // Fork a new process to execute 'ls'
        pid = fork();
        if (pid < 0) {
            printf("init: fork failed\n");
            exit(1);
        }
        if (pid == 0) {
            // Child process executes ls
            exec("ls", argv);
            // If exec fails, exit
            printf("init: exec ls failed\n");
            exit(1);
        }

        // Wait for the 'ls' process to finish
        for (;;) {
            wpid = wait((int *) 0);
            if (wpid == pid) {
                // The 'ls' command finished
                break;
            } else if (wpid < 0) {
                printf("init: wait returned an error\n");
                exit(1);
            }
        }
      sleep(40);
  
    } 


    else if (input[0] == '2') 
    {
      printf("init: starting sh\n");
      pid = fork();
      if(pid < 0){
        printf("init: fork failed\n");
        exit(1);
      }
      if(pid == 0){
        exec("sh", argv);
        printf("init: exec sh failed\n");
        exit(1);
      }

      for(;;)
        {
          // this call to wait() returns if the shell exits,
          // or if a parentless process exits.
          wpid = wait((int *) 0);
          if(wpid == pid){
            // the shell exited; restart it.
            break;
          } else if(wpid < 0){
            printf("init: wait returned an error\n");
            exit(1);
          } else {
            // it was a parentless process; do nothing.
          }
        }          
    } 
    else 
    {
        // printf("Invalid option. Please choose 1 or 2.\n");
        printf("I DON'T KNOW WHAT YOU'RE FUC*KING TALKING ABOUT !!! \n");
        printf("Shutting down XV6...\n");
        sleep(3);


    }
  }
}