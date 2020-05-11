#include <sys/syscall.h>
#include <stdio.h>
#include <signal.h>
#include <sys/debug.h>
#include <stddef.h>
#include <errno.h>

void init_init(){
  sigset_t mask;
  sigfillset(&mask);
  sigprocmask(SIG_SETMASK, &mask, NULL);
}

char shell_path[] = "/bin/shell";
char *shell_argv[] = {
  shell_path,
  NULL
};

int main(int argc, char **argv){
  struct message m;
  pid_t pid;
  int status;
  int i;
  

  init_init();

  pid = vfork();
  if(pid == 0){
    i = execv(shell_path, shell_argv);
    printf("exec failed %d\n", errno);
    return 0;
  }
  return 0;
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
