#include "../winix.h"

int do_sysconf(struct proc* who, struct message *m){
    switch(m->m1_i1){
        case _SC_CLK_TCK:
            return 60;

        default:
            return ERR;
    }
}

