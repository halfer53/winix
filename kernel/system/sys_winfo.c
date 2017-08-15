#include "../winix.h"

int do_winfo(struct proc *who, struct message *m){
    switch(m->i1){
        case WINFO_PS:
            process_overview();
            break;

        case WINFO_MEM:
            kmesg("Sys Mem bitmap: ");
            print_sysmap();
            break;
        default:
            break;
    }
    return OK;
}

