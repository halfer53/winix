//
// Created by bruce on 19/04/20.
//

#include <fs/const.h>
#include <kernel/proc.h>
#include <fs/super.h>
#include "cmake/cmake_util.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "fs_methods.h"

void init_bitmap();


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