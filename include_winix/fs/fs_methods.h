//
// Created by bruce on 4/05/20.
//

#ifndef FS_FS_METHODS_H_
#define FS_FS_METHODS_H_

#include <uchar.h>
#include <sys/stat.h>
#include <kernel/proc.h>
#include <stddef.h>
#include <fs/type.h>
#include <stdbool.h>

int sys_open(struct proc *who, const char *path,int flags, mode_t mode);
int filp_open(struct proc* who, struct filp** _filp, const char *path, int flags, mode_t mode);
int filp_close(struct filp* filp);
int filp_write(struct proc* who, struct filp* file, void *buf, size_t count);
int filp_read(struct proc* who, struct filp* file, void *buf, size_t count);

int sys_read(struct proc *who, int fd, void *buf, size_t count);
int sys_write(struct proc *who, int fd, void *buf, size_t count);
int sys_close(struct proc *who, int fd);
int sys_pipe(struct proc* who, int fd[2]);
int sys_chmod(struct proc* who, const char* pathname, mode_t mode);
int sys_chown(struct proc* who, const char* pathname, uid_t owner, gid_t group);
int sys_chdir(struct proc* who, const char* pathname);
int sys_dup(struct proc* who, int oldfd);
int sys_dup2(struct proc* who, int oldfd, int newfd);
int sys_umask(struct proc* who, mode_t mask);
int sys_lseek(struct proc* who, int fd, off_t offset, int whence);
int sys_mkdir(struct proc* who, const char* pathname, mode_t mode);
int sys_access(struct proc* who, const char* pathname, int mode);
int sys_stat(struct proc* who, const char* pathname, struct stat *statbuf);
int sys_fstat(struct proc* who, int fd, struct stat* statbuf);
int sys_link(struct proc* who, char *oldpath, char *newpath);
int sys_unlink(struct proc* who, const char* path, bool allow_dir);
int sys_mknod(struct proc* who, const char *pathname, mode_t mode, dev_t devid);
int sys_getdents(struct proc* who, int fd, struct dirent* dirp_dst, unsigned int count);
int sys_getcwd(struct proc* who, char* pathname, int size, char** result);
int sys_rmdir(struct proc* who, const char* pathname);

void init_dev();
void init_tty();
int init_dirent(inode_t* dir, inode_t* ino);
int fill_dirent(inode_t* ino, struct winix_dirent* curr, const char* string);
bool has_file_access(struct proc* who, struct inode* ino, mode_t mode);
int get_inode_by_path(struct proc* who, const char *path, struct inode** inode);
int alloc_block(inode_t *ino, struct device* id);
int makefs( char* disk_raw, size_t disk_size_words);
void init_fs();
int init_filp_by_inode(struct filp* filp, struct inode* inode);
int init_inode_non_disk(struct inode* ino, ino_t num, struct device* dev, struct superblock* sb);
void init_pipe();
int remove_inode_from_dir(struct proc* who,struct inode* dir, struct inode* target, char* name);
int get_fd(struct proc *curr, int start, int *open_slot, filp_t *fpt);
int add_inode_to_directory(struct proc* who,inode_t* dir, inode_t* ino, char* string);
// int register_device(struct device* dev, const char* name, dev_t id, mode_t type, struct device_operations*, struct filp_operations*);
int release_filp(struct filp* file);
int _release_inode(inode_t *inode, bool is_indirect_zone);
#define release_inode(ino)  _release_inode(ino, false)
filp_t *find_filp(inode_t *inode);
filp_t *get_free_filp();
void init_filp();
struct inode* get_free_inode_slot();
struct device* get_dev(dev_t dev);
int sys_creat(struct proc* who, const char* path, mode_t mode);
size_t get_inode_total_size_word(struct inode* ino);
blkcnt_t get_inode_blocks(struct inode* ino);
struct superblock* get_sb(struct device* id);
void init_inodetable();
int read_inode(int num, inode_t **inode, struct device*);
inode_t* get_inode(int num, struct device*);
int put_inode(inode_t *inode, bool is_dirty);
inode_t* alloc_inode(struct device*, struct device*);
void init_inode_proc_field(struct inode* ino, struct proc* who, mode_t devtype, mode_t mode);
int truncate_inode(inode_t *inode);
void init_inode();
bool is_inode_in_use(int num, struct device* id);
void flush_inodes();

int run_unit_tests();

int _iter_zone_init(struct zone_iterator* iter, struct inode* inode, int zone_idx);
#define iter_zone_init(iter, inode) _iter_zone_init(iter, inode, 0)

zone_t iter_zone_get_next(struct zone_iterator* iter);
int iter_zone_alloc(struct zone_iterator* iter);
int iter_zone_close(struct zone_iterator* iter);
bool iter_zone_has_next(struct zone_iterator* iter);

int _iter_dirent_init(struct dirent_iterator* iter, struct inode* inode, int zone_idx, int dir_idx, bool non_empty);
#define iter_dirent_init(iter, inode) _iter_dirent_init(iter, inode, 0, 0, false)
#define iter_dirent_init_non_empty(iter, inode) _iter_dirent_init(iter, inode, 0, 0, true)

bool iter_dirent_has_next(struct dirent_iterator* iter);
struct winix_dirent* iter_dirent_get_next(struct dirent_iterator* iter);
int iter_dirent_alloc(struct dirent_iterator* iter);
int iter_dirent_close(struct dirent_iterator* iter);

int char32_strcmp(const char32_t *s1, const char *s2);
int char32_strlen(const char32_t *s);
char32_t *char32_strlcpy(char32_t *dest, const char *src, size_t n);
char *char32_strlcpy2(char *dest, char32_t *src, size_t n);
char *char32_index(const char32_t *string, int c);

#endif //FS_FS_METHODS_H_
