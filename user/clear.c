#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"

int main() {
    // int ret = syscall(SYS_shutdown);
    // if (ret < 0) {
    //     printf("Shutdown syscall failed\n");
    // }
    clear();
    printf("\t\t====================================================\n");
    printf("\n\t\t***** Welcome to our new version of xv6 *****\n\n\n");
    printf("\t\t\t designed by M1 students of UY1\n");
    printf("\t\t===================================================\n");        
    printf("\n* execute a system call*\n");
    printf("=====================\n");
    return 0;
}
