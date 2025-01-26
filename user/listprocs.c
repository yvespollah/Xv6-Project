#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

#define INTERVAL 100  // Refresh interval in clock ticks (1 clock tick = ~10ms)
#define CTRL_A 1      // ASCII value of Ctrl+A

int main(int argc, char *argv[]) {
  int limit = 5;  // Default limit of refreshes
  if (argc > 1) {
    limit = atoi(argv[1]);  // User can specify the number of refreshes
    if (limit <= 0 || limit > 10) {
      printf("Invalid limit. Please specify a positive integer.between 1 and 10\n");
      exit();
    }
  }

  printf("Task Manager: Wait %d refreshes\n", limit);

  for (int i = 0; i < limit; i++) {
    printf("\n--- Process List (%d/%d) ---\n", i + 1, limit);
    if (listprocs() < 0) {
      printf("Error: Failed to list processes\n");
      exit();
    }
      sleep(INTERVAL*2);  // Wait for the specified interval
  }

  printf("\n\nTask Manager: Exiting after %d refreshes\n", limit);
  exit();
  return 0;
}

