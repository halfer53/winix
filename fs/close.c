#include "fs.h"


 int sys_close(int fd){

     filp_t *filp = current_proc->fp_filp[fd];
     filp->filp_ino = NIL_INODE;
     filp->filp_pos = 0;

     current_proc->fp_filp[fd] = 0;
     return 0;
 }