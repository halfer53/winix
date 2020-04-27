#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>

#define MEM_SIZE (32 * 1024)
int mem[MEM_SIZE];
int curr;

void* kmalloc(unsigned int size){
    void* ret = &mem[curr];
    curr += size;
    return ret;
}
void kfree(void *ptr){

}

int syscall_reply(int reply, int dest, struct message* m){
    printf("Syscall %d reply to %d\n", reply, dest);
}

int syscall_reply2(int syscall_num, int reply, int dest, struct message* m){
    printf("Syscall %d reply %d to %d\n", syscall_num, reply, dest);
}

