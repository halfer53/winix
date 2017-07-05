#include <sys/types.h>
#include <sys/syscall.h>

DECLARE_SYSCALL(int printf, (const char *format, ...), 
	SYSCALL_SIGNAL,
	m.p1 = (void *)format; m.p2 = (void *)((int *)&format+1);,
	OK )

	DECLARE_SYSCALL(int printf, (const char *format, ...), SYSCALL_SIGNAL, m.p1 = (void *)format; m.p2 = (void *)((int *)&format+1);, OK )

int main(int argc, char const *argv[]) {
	while(1){
		printf("I'm loaded");
	}
	
  return OK;
}
