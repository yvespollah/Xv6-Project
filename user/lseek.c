#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
int main(void) {
    int fd = open("testfile.txt", O_CREATE | O_RDWR);
    if (fd < 0) 
    {
        printf("Failed to open file\n");
        exit();
    }
    write(fd, "Hello, xv6!", 12);
    lseek(fd, 0, 0 );    
    char buf[13];
    read(fd, buf, 12);
    buf[12] = '\0';
    printf("File content: %s\n", buf);
    lseek(fd, -5, 2 );    
    read(fd, buf, 5);
    // if (offset<0)
    // {
    //     printf(1, "the cursor have reached the begining of the file");
    //     exit();
    // }
    buf[5] = '\0';
    printf("retrieve the 5 last charters: %s\n", buf );
    lseek(fd, 0, 0 );    
    //  if (offset > fd.size)
    // {
    //     printf(1, "the cursor have reached the end of the file");
    //     exit();
    // }
    read(fd, buf, 5);
    buf[5] = '\0';
    printf("retrieve the 5 first charters: %s\n", buf );
    lseek(fd, 0, 1 );    
    read(fd, buf, 2);
    buf[2] = '\0';
    printf("retrieve the 2 following charters from the current cursor place: %s\n", buf );
    
    close(fd);
    exit();
}