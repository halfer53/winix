#ifndef _FS_FS_H_
#define _FS_FS_H_ 1

#include <sys/errno.h>
#include <sys/types.h>
#include <fs/type.h>
#include <fs/const.h>
#include <winix/type.h>
#include "string.h"
#include <fs/inode.h>
#include <fs/filp.h>
#include <kernel/proc.h>
#include "cache.h"
#include <fs/dev.h>
#include "path.h"
#include "super.h"
#include <sys/debug.h>
#include <string.h>
#include <winix/bitmap.h>
#include <stdbool.h>

#define SIZE (64 * 1024)
extern size_t DISK_SIZE;
extern disk_word_t DISK_RAW[SIZE];

int sys_open(struct proc *who, char *path,int flags, mode_t mode);
int sys_read(struct proc *who, int fd, void *buf, size_t count);
int sys_write(struct proc *who, int fd, void *buf, size_t count);
int sys_close(struct proc *who, int fd);
block_t alloc_block(inode_t *ino, struct device* id);

int makefs( disk_word_t* disk_raw, disk_word_t disk_size_words);
void init_fs(disk_word_t* disk_raw, disk_word_t disk_size_words);

int get_fd(struct proc *curr, int start, int *open_slot, filp_t **fpt);
int add_inode_to_directory(inode_t* dir, inode_t* ino, char* string);

filp_t *get_filp(int fd);
filp_t *find_filp(inode_t *inode);
filp_t *get_free_filp();
void init_filp();

struct superblock* get_sb(struct device* id);
void init_inodetable();
inode_t* read_inode(int num, struct device*);
inode_t* get_inode(int num, struct device*);
int put_inode(inode_t *inode, bool is_dirty);
inode_t* alloc_inode( struct device*);
void init_inode();

void* kmalloc(unsigned int size);
void kfree(void *ptr);
int kprintf(const char *format, ...);

#ifdef FS_CMAKE
#define kprintf(...) printf(__VA_ARGS__)
#define kmalloc(...) malloc(__VA_ARGS__)
#define kfree(...) free(__VA_ARGS__)
#endif

#ifndef EOF
#define EOF (-1)
#endif
#ifndef ERR
#define ERR (-1)    /* general error flag */
#endif
#ifndef OK
#define OK 0        /* general OK flag */
#endif
#ifndef NULL
#define NULL ((void*)(0))    /* general error flag */
#endif

#ifndef _DIR_H_
#define _DIR_H_ 1

#define    DIRBLKSIZ    1024    /* size of directory block */

// each direct occupies 32 bytes, with 8 bytes for d_ino, and 24 bytes for directory name
#ifndef DIRSIZ
#define DIRSIZ    32
#endif

#ifndef DIRNAME_LEN
#define DIRNAME_LEN 29
#endif

struct dirent {
    ino_t          d_ino;       /* inode number */
    unsigned int   d_type;      /* type of file; not supported
                                   by all file system types */
#ifdef FS_CMAKE
    char    d_name[DIRNAME_LEN + 1]; /* filename */
#endif
#ifndef FS_CMAKE
    disk_word_t    d_name[DIRNAME_LEN + 1]; /* filename */
#endif
};

#define	DT_UNKNOWN	 0
#define	DT_FIFO		 1
#define	DT_CHR		 2
#define	DT_DIR		 4
#define	DT_BLK		 6
#define	DT_REG		 8
#define	DT_LNK		10
#define	DT_SOCK		12

#endif /* _DIR_H */

#define S_IFMT     0170000   /* bit mask for the file type bit field */
#define S_IFSOCK   0140000   /* socket */
#define S_IFLNK    0120000   /* symbolic link */
#define S_IFREG    0100000   /* regular file */
#define S_IFBLK    0060000   /* block device */
#define S_IFDIR    0040000   /* directory */
#define S_IFCHR    0020000   /* character device */
#define S_IFIFO    0010000   /* FIFO */

/* open-only flags */
#define    O_RDONLY     0x0000        /* open for reading only */
#define    O_WRONLY     0x0001        /* open for writing only */
#define    O_RDWR       0x0002        /* open for reading and writing */
#define    O_ACCMODE    0x0003        /* mask for above modes */
#define    O_CREAT      0x0200        /* create if nonexistant */


extern struct proc *current_proc;


#endif

