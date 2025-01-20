#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    if (unlink(argv[1]) < 0) {
        printf("Error deleting file: %s\n", argv[1]);
        exit(1);
    }

    printf("File deleted successfully\n");
    exit(1);
}
