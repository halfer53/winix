#ifndef _FS_PATH_H_
#define _FS_PATH_H_ 1

#include <stdbool.h>

char *get_name(char *old_name, char string[WINIX_NAME_LEN]);
int advance(inode_t *dirp, char string[WINIX_NAME_LEN]);
int get_parent_inode_num(inode_t *dirp);
int get_child_inode_name(inode_t* parent, inode_t* child, char string[WINIX_NAME_LEN]);
int eat_path(struct proc* who, char *path, struct inode** last_dir, struct inode** ret_ino, char string[WINIX_NAME_LEN]);
bool is_fd_opened_and_valid(struct proc* who, int fd);

#endif
