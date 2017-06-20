#include "fs.h"


// int do_close(int fd){
    
//     filp_t *filp = current_proc->fp_filp[fd];
//     free_filp(filp);
//     if(fd < 0 || fd >= NR_FILPS)
//         return 1;

//     current_proc->fp_filp[fd] = NIL_FILP;
//     return 0;
// }