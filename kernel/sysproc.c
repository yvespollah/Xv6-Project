#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "stat.h"



uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// fonction to display a message of salutation

uint64
sys_welcome(void)
{
    printf("Hey, welcome to xv6 system\n");
    return 0;
}

// Function to calculate free memory
uint64
sys_freemem(void)
{
    return free_mem_size(); // Calls the memory allocator to get free memory
}

// function that create a file


uint64
sys_touch(void)
{
    char path[MAXPATH];
    if (argstr(0, path, MAXPATH) < 0)
        return -1;

    begin_op();  // Start a transaction
    struct inode *ip = create(path, T_FILE, 0, 0);
    if (ip == 0) {
        end_op();  // End the transaction
        return -1;
    }
    iunlockput(ip);
    end_op();  // End the transaction

    return 0; // Success
}

int
sys_cps(void)
{
  return cps();
}
