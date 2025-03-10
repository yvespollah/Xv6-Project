#define NULL 0

struct stat;
struct rtcdate;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int fsize(int);
void echoOnOff();

//-------------yves - ash ---youms------------------------------------
int clear();
int getuid();
int geteuid();
void setuid(int);
int chmod(const char*, int);
int chown(const char*, int, int);
int updateDirOwner(const char*, int, int);
int cps(void);
int chpr(int pid, int priority);
int freemem(void);
int lseek(int, const int, const int);
char *mystrncat(char *dest, const char *src, uint n);
int listprocs(void);


// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
char* strncpy(char*, const char*, int);
char* safestrcpy(char*, const char*, int);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
char* strtok(char*, const char*);
char* strtok2(char*, const char*);
char* strcat(char*, const char*);
char* itoa(int, char*, int);
int abs(int);
int isEmptyString(char*);
int pow(int, int);
int getchar(void);

