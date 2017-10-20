#include <sys/syscall.h>
#include <sys/times.h>

 clock_t times(struct tms* buf) {
    struct message m;
    _syscall(TIMES,&m);
    if(buf){
        buf->tms_utime = m.m2_l2;
        buf->tms_stime = m.m2_l3;
    }
    return m.m2_l1;
}
