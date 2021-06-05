#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <unistd.h>
#include "../fs.h"

#define MEM_SIZE (32 * 1024)
int mem[MEM_SIZE];
int curr;

struct proc pcurr;
struct proc *curr_scheduling_proc;
struct proc *curr_syscall_caller;

void mock_init_proc(){
    pcurr.proc_nr = 1;
    pcurr.pid = 1;
    curr_scheduling_proc = &pcurr;
    curr_syscall_caller = curr_scheduling_proc;
}

void emulate_fork(struct proc* p1, struct proc* p2){
    int i;
    struct filp* file;
    int procnr = p2->proc_nr;
    pid_t pid = p2->pid;
    *p2 = *p1;
    p2->proc_nr = procnr;
    p2->pid = pid;
    for (i = 0; i < OPEN_MAX; ++i) {
        file = p2->fp_filp[i];
        if(file){
            file->filp_count += 1;
        }
    }
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

int send_sig(struct proc *who, int signum){
    KDEBUG(("signal %d sent to %d\n", signum, who->proc_nr));
}

void _assert(int expression, int line, char* filename) {
    if(!expression) {
        printf("\nAssert Failed at line %d in %s\n",line,filename);
        _exit(1);
    }
}

clock_t get_uptime(){
    return 0;
}

clock_t start_unix_time = 0;

void set_start_unix_time(clock_t t){
    start_unix_time = t;
}




