#ifndef _FS_FILE_H_
#define _FS_FILE_H_

#define OPEN_MAX  8

typedef struct _filp {
  mode_t filp_mode;		/* RW bits, telling how file is opened */
  int filp_flags;		/* flags from open and fcntl */
  int filp_count;		/* how many file descriptors share this slot?*/
  inode_t *filp_ino;	/* pointer to the inode */
  // off_t filp_pos;		/* file position */
  int filp_pos;		/* file position */

  /* following are for fd-type-specific select() */
  // int filp_pipe_select_ops;
}filp_t;

#define NIL_FILP (struct filp *) 0	/* indicates absence of a filp slot */

#endif
