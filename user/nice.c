#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


int
main(int argc, char *argv[])
{
  int priority, pid;
  if(argc < 3){
    printf("Usage: nice pid priority\n");
    exit();
  }
  pid = atoi(argv[1]);
  priority = atoi(argv[2]);
  if (priority < 0 || priority > 20){
    printf("Invalid priority (0-20)!\n");
    exit();
  }
  chpr(pid, priority);
  exit();
}