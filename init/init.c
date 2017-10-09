#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv){
  struct message m;
  pid_t pid;
  int status;

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
