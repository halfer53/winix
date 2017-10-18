#include <unistd.h>
#include <stdio.h>

void init_init(){
  sigset_t mask;
  sigfillset(&mask);
  sigprocmask(SIG_SETMASK, &mask, NULL);
}

int main(int argc, char **argv){
  struct message m;
  pid_t pid;
  int status;

  init_init();
  while(1){
    pid = wait(&status);
    if(pid == -1){
      break;
    }
    // printf("INIT: child %d exit status %d\n", pid, WEXITSTATUS(status));
  }
  
  printf("Shutting down...\n");
  winix_receive(&m);
  return 0;
}
