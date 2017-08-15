#include "../winix.h"

int do_winfo(struct proc *who, struct message *m){
    switch(m->i1){
        case WINFO_PS:
            process_overview();
            break;

        default:
            break;
    }
    return OK;
}

