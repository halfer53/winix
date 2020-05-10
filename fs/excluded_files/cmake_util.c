#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <unistd.h>
#include "../fs.h"

#define MEM_SIZE (32 * 1024)
int mem[MEM_SIZE];
int curr;

struct proc pcurr;
struct proc *current_proc;
struct proc *curr_user_proc_in_syscall;

void mock_init_proc(){
    pcurr.proc_nr = 1;
    pcurr.pid = 1;
    current_proc = &pcurr;
    curr_user_proc_in_syscall = current_proc;
}

void emulate_fork(struct proc* p1, struct proc* p2){
    int procnr = p2->proc_nr;
    pid_t pid = p2->pid;
    *p2 = *p1;
    p2->proc_nr = procnr;
    p2->pid = pid;
}

bool is_vaddr_ok(vptr_t* addr,struct proc* who){
    return true;
}

void* kmalloc(unsigned int size){
    void* ret = &mem[curr];
    curr += size;
    return ret;
}
void kfree(void *ptr){

}

int syscall_reply(int reply, int dest, struct message* m){
    KDEBUG(("Syscall %d reply to %d\n", reply, dest));
}

int syscall_reply2(int syscall_num, int reply, int dest, struct message* m){
    KDEBUG(("Syscall %d reply %d to Proc %d\n", syscall_num, reply, dest));
}

void _assert(int expression, int line, char* filename) {
    if(!expression) {
        printf("\nAssert Failed at line %d in %s\n",line,filename);
        _exit(1);
    }
}




