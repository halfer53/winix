#ifndef _FS_PATH_H_
#define _FS_PATH_H_ 1

// each direct occupies 32 bytes, with 8 bytes for d_ino, and 24 bytes for directory name
#ifndef DIRSIZ
#define DIRSIZ    32
#endif

#define NAME_MAX    32
#define PATH_MAX    128

char *get_name(char *old_name, char string[NAME_MAX]);
inode_t *advance(inode_t *dirp, char string[NAME_MAX]);
inode_t *last_dir(char *path, char string[DIRSIZ]);
inode_t* eat_path(char *path);

#endif