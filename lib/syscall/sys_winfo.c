#include <lib.h>


int sys_ps(){
	struct message m;
	m.i1 = WINFO_PS;
	return _syscall(SYSCALL_WINFO,&m);
}


int sys_meminfo(){
	struct message m;
	m.i1 = WINFO_MEM;
	return _syscall(SYSCALL_WINFO, &m);
}
