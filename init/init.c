#include <unistd.h>

int main(int argc, char **argv){
  struct message m;
  pid_t pid;
  int status;

  while(1){
    pid = wait(&status);
    if(pid == -1){
      exit(0);
    }
    printf("INIT: child %d exit status %d\n", pid, WEXITSTATUS(status));
  }
  
  return 0;
}
