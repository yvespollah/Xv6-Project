#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: touch <file_name>\n");
        exit(1);
    }

    int result = touch(argv[1]);  // Call the touch system call
    if (result == 0) {
        printf("File '%s' created successfully.\n", argv[1]);
    } else {
        printf("Error creating file '%s'.\n", argv[1]);
    }
    exit(0);
}
