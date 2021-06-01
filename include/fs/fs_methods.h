//
// Created by bruce on 4/05/20.
//

#ifndef FS_FS_METHODS_H
#define FS_FS_METHODS_H

#include <kernel/proc.h>
#include <sys/types.h>
#include <fs/type.h>
#include <stdbool.h>

int sys_open(struct proc *who, char *path,int flags, mode_t mode);
int sys_read(struct proc *who, int fd, void *buf, size_t count);
int sys_write(struct proc *who, int fd, void *buf, size_t count);
int sys_close(struct proc *who, int fd);
int sys_pipe(struct proc* who, int fd[2]);
int sys_chmod(struct proc* who,  char *pathname, mode_t mode);
int sys_chown(struct proc* who,  char *pathname, uid_t owner, gid_t group);
int sys_chdir(struct proc* who, char* pathname);
int sys_dup(struct proc* who, int oldfd);
int sys_dup2(struct proc* who, int oldfd, int newfd);
int sys_umask(struct proc* who, mode_t mask);
int sys_lseek(struct proc* who, int fd, off_t offset, int whence);
int sys_mkdir(struct proc* who, char* pathname, mode_t mode);
int sys_access(struct proc* who, char* pathname, int mode);
int sys_stat(struct proc* who, char *pathname, struct stat *statbuf);
int sys_fstat(struct proc* who, int fd, struct stat* statbuf);
int sys_link(struct proc* who, char *oldpath, char *newpath);
int sys_unlink(struct proc* who, char *path);
int sys_mknod(struct proc* who, char *pathname, mode_t mode, dev_t devid);
int sys_getdents(struct proc* who, int fd, struct dirent* dirp_dst, unsigned int count);

int init_dirent(inode_t* dir, inode_t* ino);
int fill_dirent(inode_t* ino, struct winix_dirent* curr, char* string);
bool has_file_access(struct proc* who, struct inode* ino, mode_t mode);
int get_inode_by_path(struct proc* who, char *path, struct inode** inode);
int alloc_block(inode_t *ino, struct device* id);
int makefs( char* disk_raw, size_t disk_size_words);
void init_fs();
int init_filp_by_inode(struct filp* filp, struct inode* inode);
int init_inode_non_disk(struct inode* ino, ino_t num, struct device* dev, struct superblock* sb);
void init_pipe();
int remove_inode_from_dir(struct proc* who,struct inode* dir, struct inode* target, char* name);
int get_fd(struct proc *curr, int start, int *open_slot, filp_t **fpt);
int add_inode_to_directory(struct proc* who,inode_t* dir, inode_t* ino, char* string);
int register_device(struct device* dev, const char* name, dev_t id, mode_t type, struct device_operations*, struct filp_operations*);
int release_filp(struct filp* file);
int release_inode(inode_t *inode);
filp_t *get_filp(int fd);
filp_t *find_filp(inode_t *inode);
filp_t *get_free_filp();
void init_filp();
struct inode* get_free_inode_slot();
struct device* get_dev(dev_t dev);
int sys_creat(struct proc* who, char* path, mode_t mode);
size_t get_inode_total_size_word(struct inode* ino);
blkcnt_t get_inode_blocks(struct inode* ino);
struct superblock* get_sb(struct device* id);
void init_inodetable();
int read_inode(int num, inode_t **inode, struct device*);
inode_t* get_inode(int num, struct device*);
int put_inode(inode_t *inode, bool is_dirty);
inode_t* alloc_inode(struct proc* who, struct device*, struct device*);
int truncate_inode(inode_t *inode);
void init_inode();
int unit_test1();

int char32_strcmp(const char32_t *s1, const char *s2);
int char32_strlen(const char32_t *s);
char32_t *char32_strlcpy(char32_t *dest, const char *src, size_t n);

#endif //FS_FS_METHODS_H
