#include <unistd.h>

int main(int argc, char **argv){
  struct message m;

  winix_receive(&m);
  
  return 0;
}
