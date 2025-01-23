#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments is provided
    if (argc < 2) {
        printf("Usage: touch <file_name>\n");
        exit();
    }

    // Call the touch system call to create a file
    int result = open(argv[1], 0x200 /* O_CREATE */); // Use `open` with O_CREATE flag

    // Check the result and print appropriate messages
    if (result >= 0) {
        printf("File '%s' created successfully.\n", argv[1]);
        close(result); // Close the file after creating it
    } else {
        printf("Error creating file '%s'.\n", argv[1]);
    }

    exit();
}
