#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* fmtname(char *path) {
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

void print_header() {
    // Print header for directory contents
    printf("\nPermissions\tInode\tOwner\tGroup\tSize\tName\n");
    printf("--------------------------------------------------------------\n");
}

void ls(char *path) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Print header for directory contents only
    if (st.type == T_DIR) {
        print_header();
    }

    switch(st.type) {
    case T_FILE:
        // For files, print a single line of info
        printf("file\t%d\t%d\t%d\t%s\n", st.ino, 0, st.size, fmtname(path));
        break;

    case T_DIR:
		
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("ls: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            struct user* fileOwner = getUserFromUid(st.uid);
            struct group* groupOwner = getGroupFromGid(st.gid);
            char permisionsString[10];
            getPermisionsString(st.mode, st.type, permisionsString);
			
            // Print directory file information with tabs for spacing
            printf("%s\t%d\t%s\t%s\t%d\t%s\n", permisionsString, st.ino, fileOwner->username, groupOwner->groupname, st.size, fmtname(buf));
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    int i;

    if(argc < 2){
        ls(".");
        exit();
    }
    for(i = 1; i < argc; i++)
        ls(argv[i]);
    exit();
}
