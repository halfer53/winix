#include "fs.h"


int sys_open(int fd, int flags, mode_t mode){
register struct inode *rip;
  int r, b, exist = TRUE;
  dev_t dev;
  mode_t bits;
  off_t pos;
  struct filp *fil_ptr, *filp2;

  
  bits = (mode_t) mode_map[oflags & O_ACCMODE];

  
  if ( (r = get_fd(0, bits, &m_in.fd, &fil_ptr)) != OK) return(r);

  
  if (oflags & O_CREAT) {
  	
        omode = I_REGULAR | (omode & ALL_MODES & fp->fp_umask);
    	rip = new_node(user_path, omode, NO_ZONE);
    	r = err_code;
    	if (r == OK) exist = FALSE;      
	else if (r != EEXIST) return(r); 
	else exist = !(oflags & O_EXCL); 
					    flag is set this is an error */
  } else {
	 
    	if ( (rip = eat_path(user_path)) == NIL_INODE) return(err_code);
  }

  
  fp->fp_filp[m_in.fd] = fil_ptr;
  fil_ptr->filp_count = 1;
  fil_ptr->filp_ino = rip;
  fil_ptr->filp_flags = oflags;

  
  if (exist) {
  	
  	if ((r = forbidden(rip, bits)) == OK) {
  		
	  	switch (rip->i_mode & I_TYPE) {
	    	   case I_DIRECTORY: 
					r = (bits & W_BIT ? EISDIR : OK);
					break;

	     	   case I_CHAR_SPECIAL:
     		   case I_BLOCK_SPECIAL:
					dev = (dev_t) rip->i_zone[0];
					r = dev_open(dev, who, bits | (oflags & ~O_ACCMODE));
					break;

				case I_NAMED_PIPE:
					oflags |= O_APPEND;	
					fil_ptr->filp_flags = oflags;
					r = pipe_open(rip, bits, oflags);
					if (r != ENXIO) {
						
						b = (bits & R_BIT ? R_BIT : W_BIT);
						fil_ptr->filp_count = 0; 
						if ((filp2 = find_filp(rip, b)) != NIL_FILP) {
							
							fp->fp_filp[m_in.fd] = filp2;
							filp2->filp_count++;
							filp2->filp_ino = rip;
							filp2->filp_flags = oflags;

							
							rip->i_count--;
						} else {
							
							fil_ptr->filp_count = 1;
							if (b == R_BIT)
								pos = rip->i_zone[V2_NR_DZONES+0];
							else
								pos = rip->i_zone[V2_NR_DZONES+1];
							fil_ptr->filp_pos = pos;
						}
					}
					break;
 		}
  	}
  }

  
  if (r != OK) {
	if (r == SUSPEND) return(r);		
	fp->fp_filp[m_in.fd] = NIL_FILP;
	fil_ptr->filp_count= 0;
	put_inode(rip);
	return(r);
  }
  
  return(m_in.fd);
}

