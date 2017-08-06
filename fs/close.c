#include "fs.h"


 int sys_close(struct proc *who,int fd){

     filp_t *filp = who->fp_filp[fd];
     filp->filp_ino = NIL_INODE;
     filp->filp_pos = 0;

	 who->fp_filp[fd] = 0;
     return OK;
 }