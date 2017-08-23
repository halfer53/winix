#include <lib.h>



int getchar(){
	struct message m;
	return _SYSCALL(SYSCALL_GETC,&m);
}
