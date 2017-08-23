#include <lib.h>

int _SYSCALL(int syscall_num, struct message *m){
	m->type = syscall_num;
	winix_sendrec(SYSTEM_TASK, m); 
	if(m->i1 < 0){
		__set_errno(-m->i1);
		return -1;
	}
	return m->i1;
}
