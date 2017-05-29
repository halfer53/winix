#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>


#define STACK_SIZE 1000    


void func(int);

ucontext_t context, *cp = &context;

int main(void) {

  int  value = 1;

  getcontext(cp);
  context.uc_link = 0;
  if ((context.uc_stack.ss_sp = (char *) malloc(STACK_SIZE)) != NULL) {
    context.uc_stack.ss_size = STACK_SIZE;
    context.uc_stack.ss_flags = 0;
             errno = 0;
    makecontext(cp,func,1,value);
              if(errno != 0){
                perror("Error reported by makecontext()");
                return -1;         /* Error occurred exit */ }
  }
  else {
    perror("not enough storage for stack");
    abort();
  }
  printf("context has been built\n");
  setcontext(cp);
  perror("returned from setcontext");
  abort();

}