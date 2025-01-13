#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(void)
{
    int free_memory = freemem(); // Call the system call
    printf("Free memory: %d bytes\n", free_memory);
    exit(0);
}
