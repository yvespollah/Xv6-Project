#include "kernel/types.h"
#include "user/user.h"

int main(){
    printf("Listing all processes:\n");
    listprocs();
    exit(0);
}