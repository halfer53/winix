#include <fs/fs.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char DISK_RAW[DISK_SIZE];

#define MEM_SIZE (1024 * 1024)
char mem[MEM_SIZE];
int curr;

struct proc pcurr;
struct proc *curr_scheduling_proc;
struct proc *curr_syscall_caller;

void init_disk(){
    int ret;
    memset(DISK_RAW, 0, DISK_SIZE);
    ret = makefs(DISK_RAW, DISK_SIZE);
    assert(ret == 0);
}

void mock_init_proc(){
    memset(&pcurr, 0, sizeof(struct proc));
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

bool is_vaddr_ok(vptr_t* addr, size_t len, struct proc* who){
    return true;
}

void* kmalloc(size_t nitimes, size_t size){
    void *ret;
    size_t total = nitimes * size;
    if (curr + total >= MEM_SIZE){
        return NULL;
    }
    ret = &mem[curr];
    curr += total;
    return ret;
}
void kfree(void *ptr){

}

int syscall_reply(int reply, int dest, struct message* m){
    kdebug("Syscall %d reply to %d\n", reply, dest);
    return 0;
}

int syscall_reply2(int syscall_num, int reply, int dest, struct message* m){
    kdebug("Syscall %d reply %d to Proc %d\n", syscall_num, reply, dest);
    return 0;
}

int send_sig(struct proc *who, int signum){
    kdebug("signal %d sent to %d\n", signum, who->proc_nr);
    sigaddset(&who->sig_pending, signum);
    // who->sig_pending |= 1 << signum;
    return 0;
}

void _assert(int expression, int line, char* filename) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    if(!expression) {
        char *p = NULL;
        printf("\nAssert Failed at line %d in %s\n",line,filename);
        // trigger segment fault in order to inspect variables in debugger
        p = 0;
    }
#pragma GCC diagnostic pop
}

void _panic(const char* str, const char* file) {
    printf("\r\nPanic! ");

    if(str)
        printf("%s", str);

    if(file)
        printf(" in %s\n", file);
    abort();
}


clock_t get_uptime(){
    return 0;
}

clock_t start_unix_time = 0;

void set_start_unix_time(clock_t t){
    start_unix_time = t;
}

int tty_write_rex(RexSp_t* rex, char* data, size_t len){
    return 0;
}

const char* kstr_error(int err){
    if (err == 0)
        return "0";
    if (err == DONTREPLY)
        return "DONTREPLY";
    if (err == SUSPEND)
        return "SUSPEND";
    return strerror(err);
}

int copy_from_user(struct proc* who, void *dest, vptr_t *src, size_t len){
    memcpy(dest, src, len);
    return 0;
}


