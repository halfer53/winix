#ifndef _STATFS_H_
#define _STATFS_H_

struct statfs {
    int f_type;    /* Type of filesystem (see below) */
    int f_bsize;   /* Optimal transfer block size */
    int f_blocks;  /* Total data blocks in filesystem */
    int f_bfree;   /* Free blocks in filesystem */
    int f_bavail;  /* Free blocks available to
                            unprivileged user */
    int f_files;   /* Total inodes in filesystem */
    int f_ffree;   /* Free inodes in filesystem */
    int f_namelen; /* Maximum length of filenames */
};



#endif


