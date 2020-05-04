#ifndef _FS_FILP_H_
#define _FS_FILP_H_ 1

#include <fs/inode.h>
#include <winix/comm.h>

#define OPEN_MAX  8



typedef struct filp {
    mode_t filp_mode;        /* RW bits, telling how file is opened */
    int filp_flags;        /* flags from open and fcntl */
    int filp_count;        /* how many file descriptors share this slot?*/
    inode_t *filp_ino;    /* pointer to the inode */
    off_t filp_pos;        /* file position */
    struct device* filp_dev;
    off_t pipe_read_pos;

    /* following are for fd-type-specific select() */
    // int filp_pipe_select_ops;
    int filp_table_index;
    struct list_head filp_list; // list for all the filp referring to the same underlying inode
    zone_t getdent_zone_nr;
    int getdent_dirstream_nr;
    
}filp_t;

struct filp_operations{
    int (*lseek) ( struct filp *, off_t, int);
    int (*read) (struct filp *, char *, size_t, off_t );
    int (*write) (struct filp *, char *, size_t, off_t );
    int (*readdir) (struct filp *, void *);
    int (*open) (struct inode *, struct filp *);
    int (*flush) (struct filp *);
    int (*close) (struct inode *, struct filp *);
};

#define NIL_FILP (filp_t *) 0    /* indicates absence of a filp slot */



#endif

