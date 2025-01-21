//
// Console input and output, to the uart.
// Reads are line at a time.
// Implements special input characters:
//   newline -- end of line
//   control-h -- backspace
//   control-u -- kill line
//   control-d -- end of file
//   control-p -- print process list
//

#define CMD_HISTORY 16   // Nombre maximal de commandes dans l'historique
#define CMD_MAX_LEN 128  // Longueur maximale d'une commande


#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"


#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x



char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}



int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}



char command_history[CMD_HISTORY][CMD_MAX_LEN]; // Buffer pour l'historique
int history_head = 0;  // Index de la prochaine commande à sauvegarder
int history_tail = 0;  // Index de la commande la plus ancienne
int history_index = -1; // Index courant pour naviguer (flèches)

void save_command_to_history(char *cmd) {
  if (strlen(cmd) == 0) {
    return; // Ne pas sauvegarder les commandes vides
  }
  strcpy(command_history[history_head], cmd);
  history_head = (history_head + 1) % CMD_HISTORY;
  if (history_head == history_tail) {
    history_tail = (history_tail + 1) % CMD_HISTORY; // Écraser la plus ancienne
  }
}


//
// send one character to the uart.
// called by printf(), and to echo input characters,
// but not from write().
//
void
consputc(int c)
{
  if(c == BACKSPACE){
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b'); uartputc_sync(' '); uartputc_sync('\b');
  } else {
    uartputc_sync(c);
  }
}

struct {
  struct spinlock lock;
  
  // input
#define INPUT_BUF_SIZE 128
  char buf[INPUT_BUF_SIZE];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} cons;

//
// user write()s to the console go here.
//
int
consolewrite(int user_src, uint64 src, int n)
{
  int i;

  for(i = 0; i < n; i++){
    char c;
    if(either_copyin(&c, user_src, src+i, 1) == -1)
      break;
    uartputc(c);
  }

  return i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dist indicates whether dst is a user
// or kernel address.
//
int
consoleread(int user_dst, uint64 dst, int n)
{
  uint target;
  int c;
  char cbuf;

  target = n;
  acquire(&cons.lock);
  while(n > 0){
    // wait until interrupt handler has put some
    // input into cons.buffer.
    while(cons.r == cons.w){
      if(killed(myproc())){
        release(&cons.lock);
        return -1;
      }
      sleep(&cons.r, &cons.lock);
    }

    c = cons.buf[cons.r++ % INPUT_BUF_SIZE];

    if(c == C('D')){  // end-of-file
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      break;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    if(either_copyout(user_dst, dst, &cbuf, 1) == -1)
      break;

    dst++;
    --n;

    if(c == '\n'){
      // a whole line has arrived, return to
      // the user-level read().
      break;
    }
  }
  release(&cons.lock);

  return target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
void
consoleintr(int c)
{
  acquire(&cons.lock);

  switch(c){
  case C('P'):  // Print process list.
    procdump();
    break;
  case C('U'):  // Kill line.
    while(cons.e != cons.w &&
          cons.buf[(cons.e-1) % INPUT_BUF_SIZE] != '\n'){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  case C('H'): // Backspace
  case '\x7f': // Delete key
    if(cons.e != cons.w){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;

  case 0xE2: // Flèche haut
  if (history_index == -1) {
    history_index = history_head - 1;
    if (history_index < 0) {
      history_index += CMD_HISTORY;
    }
  } else if (history_index != history_tail) {
    history_index = (history_index - 1 + CMD_HISTORY) % CMD_HISTORY;
  }

  if (command_history[history_index][0] != '\0') {
    // Effacer la ligne actuelle
    while (cons.e > cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }

    // Charger la commande depuis l'historique
    char *cmd = command_history[history_index];
    for (int i = 0; cmd[i] != '\0'; i++) {
      cons.buf[cons.e++ % INPUT_BUF_SIZE] = cmd[i];
      consputc(cmd[i]);
    }
  }
  break;

case 0xE3: // Flèche bas
  if (history_index != -1) {
    history_index = (history_index + 1) % CMD_HISTORY;
  }

  // Effacer la ligne actuelle
  while (cons.e > cons.w) {
    cons.e--;
    consputc(BACKSPACE);
  }

  if (history_index == history_head || command_history[history_index][0] == '\0') {
    history_index = -1; // Réinitialiser si hors historique
  } else {
    // Charger la commande depuis l'historique
    char *cmd = command_history[history_index];
    for (int i = 0; cmd[i] != '\0'; i++) {
      cons.buf[cons.e++ % INPUT_BUF_SIZE] = cmd[i];
      consputc(cmd[i]);
    }
  }
  break;
  default:
    if(c != 0 && cons.e-cons.r < INPUT_BUF_SIZE){
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % INPUT_BUF_SIZE] = c;

      if(c == '\n' || c == C('D') || cons.e-cons.r == INPUT_BUF_SIZE){
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    if (c == '\n') {
  cons.buf[cons.e % INPUT_BUF_SIZE] = '\0'; // Terminer la commande
  save_command_to_history(cons.buf + cons.w % INPUT_BUF_SIZE); // Sauvegarder
  cons.w = cons.e; // Mettre à jour l'index d'écriture
  wakeup(&cons.r); // Réveiller le lecteur
}

    break;
  }
  
  release(&cons.lock);
}

void
consoleinit(void)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].read = consoleread;
  devsw[CONSOLE].write = consolewrite;
}
