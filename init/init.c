#include <sys/syscall.h>
#include <stdio.h>
#include <signal.h>
#include <sys/debug.h>
#include <stddef.h>
#include <errno.h>
#include <sys/tty.h>
#include <sys/fcntl.h>
#include <sys/ipc.h>
#include <fs/common.h>
#include <sys/ioctl.h>

char shell_path[] = "/bin/bash";
char *shell_argv[] = {
    shell_path,
    NULL
};

int pgid;

#define CHECK_SYSCALL(cond) \
  if (!(cond))              \
  {                         \
    failed_init(__LINE__);  \
  }

void failed_init(int line)
{
  printf("init %d failed at %d, err %s\n", getpid(), line, strerror(line));
  exit(1);
}

void block_signals()
{
  int i;
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGSEGV);
  sigprocmask(SIG_SETMASK, &mask, NULL);
  for(i = 1; i < _NSIG; i++){
    signal(i, SIG_IGN);
  }
}

void init_dev(){
  int fd, ret;

  ret = mkdir("/dev", 0x755);
  CHECK_SYSCALL(ret == 0);

  ret = mknod("/dev/tty1", 0x755, TTY1_DEV_NUM);
  CHECK_SYSCALL(ret == 0);

  ret = mknod("/dev/tty2", 0x755, TTY2_DEV_NUM);
  CHECK_SYSCALL(ret == 0);

  ret = mknod("/dev/sda", 0x755, ROOT_DEV);
  CHECK_SYSCALL(ret == 0);

}

void init_tty(){
  int fd, ret;
  fd = open("/dev/tty1", O_RDONLY);
  CHECK_SYSCALL(fd == 0);
  ret = dup(fd);
  ret = dup(fd);
  setpgid(0, 0);
  pgid = getpgid(0);
  ioctl(STDIN_FILENO, TIOCSPGRP, pgid);
}

void write_dummy_file(){
  int fd, ret, i, j;
  char c = 'a';
  char buffer[128];
  char *p = buffer;

  fd = open("/foo", O_CREAT | O_RDWR, 0x755);
  CHECK_SYSCALL(fd > 0);
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < 4; j++)
    {
      *p++ = c++;
    }
    *p = '\0';
    dprintf(fd, "%s\n", buffer);
    p = buffer;
  }
  ret = close(fd);
  CHECK_SYSCALL(ret == 0);
}

void start_init_routine(){
  pid_t pid;
  int status;
  block_signals();
  while (1)
  {
    pid = wait(&status);
    if (pid == -1)
    {
      break;
    }
    // printf("INIT: child %d exit status %d\n", pid, WEXITSTATUS(status));
  }

  printf("Shutting down...\n");
}

int main(int argc, char **argv)
{
  struct message m;
  struct stat statbuf;
  pid_t pid;
  int i, ret, fd, read_nr;

  init_dev();
  init_tty();
  
  pid = vfork();
  if (pid == 0)
  {
    execv(shell_path, shell_argv);
    return 1;
  }

  // delete bash after execving it
  unlink("/bin/init");
  unlink(shell_path);
  write_dummy_file();
  
  start_init_routine();
  return 0;
}
