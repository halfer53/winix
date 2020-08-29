#include <sys/syscall.h>
#include <stdio.h>
#include <signal.h>
#include <sys/debug.h>
#include <stddef.h>
#include <errno.h>
#include <sys/tty.h>
#include <sys/fcntl.h>

#define CHECK_SYSCALL(cond) \
if(!(cond)){ \
  failed_init(__LINE__); \
}

void failed_init(int line){
  printf("init %d failed at %d, err %s\n", getpid(), line, strerror(line));
  exit(1);
}

void init_init(){
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGSEGV);
  sigprocmask(SIG_SETMASK, &mask, NULL);
  enable_syscall_tracing();
  sync();
}

void do_ps(){
  struct message m;
  m.m1_i1 = WINFO_PS;
  _syscall(WINFO, &m);
}



char shell_path[] = "/bin/bash";
char *shell_argv[] = {
  shell_path,
  NULL
};

int main(int argc, char **argv){
  struct message m;
  struct stat statbuf;
  pid_t pid;
  int status;
  char c = 'a';
  int i, j, ret, fd, read_nr;
  char buffer[128];
  char *p = buffer;

  

  ret = mkdir("/dev", 0x755);
  CHECK_SYSCALL(ret == 0);

  ret = mknod("/dev/tty1", 0x755, TTY1_DEV_NUM);
  CHECK_SYSCALL(ret == 0);

  ret = mknod("/dev/tty2", 0x755, TTY2_DEV_NUM);
  CHECK_SYSCALL(ret == 0);

  fd = open("/dev/tty1", O_RDONLY);
  CHECK_SYSCALL(fd == 0);

  ret = dup(fd);
  ret = dup(fd);

  fd = open("/foo", O_CREAT | O_RDWR, 0x755);
  CHECK_SYSCALL(fd > 0);
  for(i = 0; i < 6; i++){
    for(j = 0; j < 4; j++){
      *p++ = c++;
    }
    *p = '\0';
    dprintf(fd, "%s\n", buffer);
    p = buffer;
  }
  ret = close(fd);
  CHECK_SYSCALL(ret == 0);

  sync();

  pid = vfork();
  if(pid == 0){
    i = execv(shell_path, shell_argv);
    return 1;
  }

  while(1){
    pid = wait(&status);
    if(pid == -1){
      break;
    }
    // printf("INIT: child %d exit status %d\n", pid, WEXITSTATUS(status));
  }
  
  printf("Shutting down...\n");
  // winix_receive(&m);
  return 0;
}
