//
// Created by bruce on 19/04/20.
//

#include <kernel/proc.h>
#include <fs/super.h>
#include "cmake/cmake_util.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE (64 * 1024)
extern size_t DISK_SIZE;
extern disk_word_t DISK_RAW[SIZE];
void init_bitmap();

int sys_open(struct proc *who, char *path,int flags, mode_t mode);
int sys_read(struct proc *who, int fd, void *buf, size_t count);
int sys_write(struct proc *who, int fd, void *buf, size_t count);
int sys_close(struct proc *who, int fd);
int sys_pipe(struct proc* who, int fd[2]);
int sys_chmod(struct proc* who,  char *pathname, mode_t mode);
int sys_chown(struct proc* who,  char *pathname, uid_t owner, gid_t group);
int sys_chdir(struct proc* who, char* pathname);
int sys_dup(struct proc* who, int oldfd);
int sys_umask(struct proc* who, mode_t mask);
int sys_lseek(struct proc* who, int fd, off_t offset, int whence);
int sys_mkdir(struct proc* who, char* pathname, mode_t mode);
int sys_access(struct proc* who, char* pathname, int mode);
int sys_stat(struct proc* who, char *pathname, struct stat *statbuf);
int sys_fstat(struct proc* who, int fd, struct stat* statbuf);
int sys_link(struct proc* who, char *oldpath, char *newpath);
int sys_unlink(struct proc* who, char *path);
int sys_getdent(struct proc* who, int fd, struct dirent* dirp_dst);

int init_dirent(inode_t* dir, inode_t* ino);
int fill_dirent(inode_t* ino, struct dirent* curr, char* string);
int get_inode_by_path(struct proc* who, char *path, struct inode** inode);
int alloc_block(inode_t *ino, struct device* id);
int makefs( disk_word_t* disk_raw, disk_word_t disk_size_words);
void init_fs();
int init_filp_by_inode(struct filp* filp, struct inode* inode);
int init_inode_non_disk(struct inode* ino, ino_t num, struct device* dev, struct superblock* sb);
void init_pipe();
int remove_inode_from_dir(struct inode* dir, struct inode* target);
int get_fd(struct proc *curr, int start, int *open_slot, filp_t **fpt);
int add_inode_to_directory(inode_t* dir, inode_t* ino, char* string);
int register_device(struct device* dev, const char* name, dev_t id, mode_t type);
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
inode_t* read_inode(int num, struct device*);
inode_t* get_inode(int num, struct device*);
inode_t* alloc_inode(struct proc* who, struct device*);
void init_inode();

struct proc pcurr, pcurr2;
struct proc *current_proc;
struct proc *curr_user_proc_in_syscall;
struct superblock *sb;


disk_word_t DISK_RAW[SIZE];
size_t DISK_SIZE;

void emulate_fork(struct proc* p1, struct proc* p2){
    int procnr = p2->proc_nr;
    pid_t pid = p2->pid;
    *p2 = *p1;
    p2->proc_nr = procnr;
    p2->pid = pid;
}

void write_disk(){
    int fd;
    int size = SIZE;
    char str[] = "#include \"fs.h\" \n size_t DISK_SIZE = 65536; disk_word_t DISK_RAW[65536];\n ";
    char str2[] = "unsigned int shell_code[] = {";
    char str3[] = "};\n";
    FILE *fp = fopen("disk.c", "w");
    fprintf(fp, "%s%s", str, str2);

    for(int i = 0; i < DISK_SIZE; i++){
        unsigned int val = DISK_RAW[i];
        fprintf(fp, "\t%08x,\n", val);
    }
    fprintf(fp, "%s\n\n", str3);
    fclose(fp);
}

int do_tests(){
    int ret, fd;
    int pipe_fd[2];
    struct superblock sb;
    DISK_SIZE = SIZE;
    init_bitmap();
    char *filename = "/foo.txt";
    char buffer[100];
    pcurr.proc_nr = 1;
    pcurr.pid = 1;
    pcurr2.proc_nr = 2;
    pcurr2.pid = 2;


    ret = makefs(DISK_RAW, DISK_SIZE);
    if(ret){
        printf("makefs failed");
        return 1;
    }
    write_disk();


    current_proc = &pcurr;
    curr_user_proc_in_syscall = current_proc;

    init_fs();
    fd = sys_open(current_proc, filename ,O_CREAT | O_RDWR, 0775);
    assert(fd == 0);

    ret = sys_write(current_proc, fd, "abc", 3);
    assert(ret == 3);
    ret = sys_write(current_proc, fd, "def", 4);
    assert(ret == 4);
    ret = sys_read(current_proc, fd, buffer, 100);
    assert(ret == 0);
    ret = sys_lseek(current_proc,fd, 0, SEEK_SET);
    assert(ret == 0);
    ret = sys_read(current_proc, fd, buffer, 100);
    assert(ret == 7);
    assert(strcmp(buffer, "abcdef") == 0);
    ret = sys_close(current_proc, fd);
    assert(ret == 0);

    fd = sys_open(current_proc, filename ,O_RDONLY, 0775);
    assert(fd == 0);

    ret = sys_read(current_proc, fd, buffer, 100);
    assert(ret == 7);
    assert(strcmp(buffer, "abcdef") == 0);
    ret = sys_read(current_proc, fd, buffer, 100);
    assert(ret == 0);
//    sys_close(current_proc, fd);

    ret = sys_pipe(&pcurr, pipe_fd);
    assert(ret == 0);
    emulate_fork(&pcurr, &pcurr2);
    ret = sys_read(&pcurr, pipe_fd[0], buffer, 100);
    assert(ret == SUSPEND);
    ret = sys_write(&pcurr2, pipe_fd[1], "1234", 4);
    assert(ret == 4);

    ret = sys_write(&pcurr2, pipe_fd[1], "1234", 5);
    assert(ret == 5);
    ret = sys_read(&pcurr, pipe_fd[0], buffer, 100);
    assert(ret == 5);
    assert(strcmp(buffer, "1234") == 0);

    ret = sys_mkdir(current_proc, "/dev", 0x755);
    assert(ret == 0);
    printf("Do LS: ");
    do_ls("/");

    ret = sys_access(current_proc, "/dev/bar.txt", F_OK);
    assert(ret != 0);

    ret = sys_creat(current_proc, "/dev/bar.txt", 0x777);
    assert(ret > 0);

    ret = sys_access(current_proc, "/dev/bar.txt", F_OK);
    assert(ret == 0);

    ret = sys_chdir(current_proc, "/dev");
    assert(ret == 0);

    printf("Do dev LS: ");
    do_ls("/dev");
    do_ls(".");
}

int main(int argc, char** argv){

    do_tests();
}