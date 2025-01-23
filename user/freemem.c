#include "kernel/types.h"
#include "user/user.h"


int main(void) {
    uint mem = freemem(); // Call the freemem system call
    printf("Free Memory : %d bytes\n", mem); // Print free memory in bytes
    exit();
}
