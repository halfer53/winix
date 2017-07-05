#ifndef _FS_FILP_H_
#define _FS_FILP_H_ 1

#include "proc.h"
#include "file.h"

int get_fd(proc_t *curr,int start, int *k, filp_t **fpt);
filp_t *get_filp(int fd);
filp_t *find_filp(inode_t *inode);
filp_t *get_free_filp();
void init_filp();

#endif