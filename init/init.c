#include <unistd.h>

int main(int argc, char **argv){
  struct message m;
  pid_t pid;
  int status;

  while(1){
    pid = wait(&status);
    if(pid == -1){
      goto end;
    }
    // printf("INIT: child %d exit status %d\n", pid, WEXITSTATUS(status));
  }
  
  end:
  printf("Shutting down\n");
  return 0;
}
