#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(void) {
  int n = 5; // Number of child processes to create
  
  printf("\nSpawning %d child processes with memory activity\n", n);
  printf("Press the \033[1;33mEnter\033[0m key to proceed\n");

  for (int i = 0; i < n; i++) {
    if (fork() == 0) {
      // Child process: simulate work
      printf("Child process %d started\n", getpid());

      while (1) {
        // Allocate memory
        char *mem = malloc(1024 * 10); // 10 KB
        if (mem == 0) {
          printf("Child process %d: memory allocation failed\n", getpid());
          exit();
        }

        // Simulate work on the allocated memory
        for (int j = 0; j < 1024 * 10; j++) {
          mem[j] = j % 256; // Write some data
        }

        // Free memory
        free(mem);

        // Simulate CPU activity
        for (volatile int j = 0; j < 1000000; j++) {
          // Busy loop to simulate CPU usage
        }
      }

      exit(); // Should never reach here
    }
  }

  // Parent process waits indefinitely
  while (1) {
    sleep(100);
  }

  return 0;
}
