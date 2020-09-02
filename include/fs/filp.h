#ifndef _FS_FILP_H_
#define _FS_FILP_H_ 1

#include <fs/inode.h>
#include <winix/comm.h>
#include <winix/type.h>

#define OPEN_MAX  16

struct filp_pipe{
    char* data;
    int mode;
    off_t pos;
};

typedef struct filp {
    mode_t filp_mode;        /* RW bits, telling how file is opened */
    unsigned int filp_flags;        /* flags from open and fcntl */
    int filp_count;        /* how many file descriptors share this slot?*/
    inode_t *filp_ino;    /* pointer to the inode */
    off_t filp_pos;        /* file position */
    struct device* filp_dev;
//    off_t pipe_read_pos;
    int pipe_mode;
    struct filp_pipe* pipe;

    /* following are for fd-type-specific select() */
    // int filp_pipe_select_ops;
    int filp_table_index;
    zone_t getdents_zone_nr;
    int getdents_dirstream_nr;

    void* private;
    
}filp_t;

struct filp_operations{
    int (*open) (struct device *, struct filp *);
    int (*read) (struct filp *, char *, size_t, off_t );
    int (*write) (struct filp *, char *, size_t, off_t );
    int (*close) (struct device *, struct filp *);
    int (*ioctl) (struct filp *, int, ptr_t*);
    // int (*lseek) ( struct filp *, off_t, int);
    // int (*flush) (struct filp *);
};

#define NIL_FILP (filp_t *) 0    /* indicates absence of a filp slot */

#define FILP_PIPE_READ  (2)
#define FILP_PIPE_WRITE (0)

#endif

