#ifndef _FS_PATH_H_
#define _FS_PATH_H_ 1

#include <stdbool.h>

// each direct occupies 32 bytes, with 8 bytes for d_ino, and 24 bytes for directory name
#ifndef DIRSIZ
#define DIRSIZ    32
#endif

#define NAME_MAX    32
#define PATH_MAX    128

char *get_name(char *old_name, char string[NAME_MAX]);
int advance(inode_t *dirp, char string[NAME_MAX]);
int get_parent_inode_num(inode_t *dirp);
int get_child_inode_name(inode_t* parent, inode_t* child, char string[NAME_MAX]);
//inode_t *last_dir(struct inode* inode, char *path, char string[DIRSIZ]);
int eat_path(struct proc* who, char *path, struct inode** last_dir, struct inode** ret_ino, char string[DIRSIZ]);
bool is_fd_opened_and_valid(struct proc* who, int fd);

#endif
