#include <kernel/kernel.h>
#include <unistd.h>

int do_sysconf(struct proc* who, struct message *m){
    switch(m->m1_i1){
        case _SC_CLK_TCK:
            return get_hz();

        default:
            return EINVAL;
    }
}

