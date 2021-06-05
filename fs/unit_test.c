//
// Created by bruce on 9/05/20.
//
#include "fs.h"
#include <assert.h>

const char * dirent_array[] = {
        ".",
        "..",
        "bar.txt",
        "bar2.txt"
};

int file_size(struct proc* who, int fd){
    struct stat statbuf;
    int ret = sys_fstat(who, fd, &statbuf);
    return statbuf.st_size;
}

char buffer[PAGE_LEN];
char b2[PAGE_LEN];

int unit_test1(){
    struct proc pcurr2;
    int ret, fd, fd2, fd3, fd4, i;
    int pipe_fd[2];
    struct stat statbuf, statbuf2;
    init_bitmap();
    char *filename = "/foo.txt";

    pcurr2.pid = 2;
    pcurr2.proc_nr = 2;

    init_dev();
    init_fs();
    init_tty();
    init_drivers();

    mock_init_proc();
    fd = sys_open(curr_scheduling_proc, filename ,O_CREAT | O_RDWR, 0775);
    ASSERT(fd == 0);
    fd2 = sys_dup(curr_scheduling_proc, fd);
    ASSERT(fd2 == fd + 1);

    ret = sys_write(curr_scheduling_proc, fd, "abc", 3);
    ASSERT(ret == 3);
    ret = sys_write(curr_scheduling_proc, fd, "def", 4);
    ASSERT(ret == 4);
    ret = sys_read(curr_scheduling_proc, fd, buffer, 100);
    ASSERT(ret == 0);
    ret = sys_lseek(curr_scheduling_proc, fd2, 0, SEEK_SET);
    ASSERT(ret == 0);
    ret = sys_read(curr_scheduling_proc, fd, buffer, 100);
    ASSERT(ret == 7);
    ASSERT(strcmp(buffer, "abcdef") == 0);
    ret = sys_close(curr_scheduling_proc, fd);
    ASSERT(ret == 0);
    ret = sys_close(curr_scheduling_proc, fd2);
    ASSERT(ret == 0);


    fd = sys_open(curr_scheduling_proc, filename ,O_RDONLY, 0x0775);
    ASSERT(fd == 0);
    ASSERT(file_size(curr_scheduling_proc, fd) == 7);
    ret = sys_read(curr_scheduling_proc, fd, buffer, 100);
    ASSERT(ret == 7);
    ASSERT(strcmp(buffer, "abcdef") == 0);
    ret = sys_read(curr_scheduling_proc, fd, buffer, 100);
    ASSERT(ret == 0);
//    sys_close(curr_scheduling_proc, fd);

    ret = sys_pipe(curr_scheduling_proc, pipe_fd);
    ASSERT(ret == 0);
    emulate_fork(curr_scheduling_proc, &pcurr2);
    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer, 100);
    ASSERT(ret == SUSPEND);
    ret = sys_write(&pcurr2, pipe_fd[1], "1234", 5);
    ASSERT(ret == 5);
    ASSERT(strcmp(buffer, "1234") == 0);
    buffer[0] = 0;

    ret = sys_write(&pcurr2, pipe_fd[1], "1234", 5);
    ASSERT(ret == 5);
    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer, 100);
    ASSERT(ret == 5);
    ASSERT(strcmp(buffer, "1234") == 0);

    memset(buffer, 'a', PAGE_LEN - 1);
    buffer[PAGE_LEN - 1] = 0;
    ret = sys_write(&pcurr2, pipe_fd[1], buffer, PAGE_LEN);
    ASSERT(ret == PAGE_LEN);
    ret = sys_write(&pcurr2, pipe_fd[1], "abc", 4);
    ASSERT(ret == SUSPEND);
    ret = sys_read(curr_scheduling_proc, pipe_fd[0], b2, PAGE_LEN);
    ASSERT(ret == PAGE_LEN);
    ASSERT(strcmp(buffer, b2) == 0);
    ret = sys_read(curr_scheduling_proc, pipe_fd[0], b2, PAGE_LEN);
    ASSERT(ret == 4);
    ASSERT(strcmp(b2, "abc") == 0);

    sys_close(curr_scheduling_proc, pipe_fd[0]);
    ret = sys_write(&pcurr2, pipe_fd[1], "a", 2);
    ASSERT(ret == 2);

    sys_close(&pcurr2, pipe_fd[0]);
    ret = sys_write(&pcurr2, pipe_fd[1], "a", 2);
    ASSERT(ret == SUSPEND);

    pcurr2.sig_table[SIGPIPE].sa_handler = SIG_IGN;
    ret = sys_write(&pcurr2, pipe_fd[1], "a", 2);
    ASSERT(ret == EPIPE);

    ret = sys_close(curr_scheduling_proc, pipe_fd[1]);
    ASSERT(ret == 0);
    ret = sys_close(&pcurr2, pipe_fd[1]);
    ASSERT(ret == 0);
//    ret = sys_write(&pcurr2, pipe_fd[1], "abc", 4);
//    ret = sys_close(&pcurr2, pipe_fd[1]);
//    ASSERT(ret == 0);
//    ret = sys_read(curr_scheduling_proc, pipe_fd[0], buffer, PAGE_LEN);
//    ASSERT(ret == 0);

    ret = sys_access(curr_scheduling_proc, "/dev", F_OK);
    ASSERT(ret != 0);

    ret = sys_mkdir(curr_scheduling_proc, "/dev", 0x755);
    ASSERT(ret == 0);

    ret = sys_access(curr_scheduling_proc, "/dev", F_OK);
    ASSERT(ret == 0);

    ret = sys_access(curr_scheduling_proc, "/dev/bar.txt", F_OK);
    ASSERT(ret != 0);

    ret = sys_creat(curr_scheduling_proc, "/dev/bar.txt", 0x777);
    ASSERT(ret > 0);

    ret = sys_access(curr_scheduling_proc, "/dev/bar.txt", F_OK);
    ASSERT(ret == 0);

    ret = sys_chdir(curr_scheduling_proc, "/dev");
    ASSERT(ret == 0);

    ret = sys_stat(curr_scheduling_proc, "/dev", &statbuf);
    ASSERT(ret == 0);
    ASSERT(curr_scheduling_proc->fp_workdir->i_num == statbuf.st_ino);

    ret = sys_link(curr_scheduling_proc, "/dev/bar.txt", "/dev/bar2.txt");
    ASSERT(ret == 0);

    ret = sys_chmod(curr_scheduling_proc, "/dev/bar.txt", 0x777);
    ASSERT(ret == 0);

    ret = sys_stat(curr_scheduling_proc, "/dev/bar.txt", &statbuf);
    ASSERT(ret == 0);

    ret = sys_stat(curr_scheduling_proc, "/dev/bar2.txt", &statbuf2);
    ASSERT(ret == 0);
    ASSERT(statbuf.st_ino == statbuf2.st_ino);
    ASSERT(statbuf.st_dev == statbuf2.st_dev);
    ASSERT(statbuf.st_nlink == 2);
    ASSERT(statbuf.st_mode == 0x777);

    fd3 = sys_open(curr_scheduling_proc, "/dev", O_RDONLY, 0);
    ASSERT(fd3 >= 0);
    struct dirent dir[4];

    ret = sys_getdents(curr_scheduling_proc, fd3, dir, 5);
    ASSERT(ret == sizeof(struct dirent) * 4);
    for (i = 0; i < 4; ++i) {
        ASSERT(char32_strcmp(dir[i].d_name, dirent_array[i]) == 0);
    }
    ret = sys_getdents(curr_scheduling_proc, fd3, dir, 10);
    ASSERT(ret == 0);

    ret = sys_unlink(curr_scheduling_proc, "/dev/bar2.txt");
    ASSERT(ret == 0);

    ret = sys_stat(curr_scheduling_proc, "/dev/bar.txt", &statbuf);
    ASSERT(ret == 0);
    ASSERT(statbuf.st_nlink == 1);

    ret = sys_stat(curr_scheduling_proc, "/dev/bar2.txt", &statbuf2);
    ASSERT(ret == ENOENT);

    ret = sys_unlink(curr_scheduling_proc, "/dev/bar.txt");
    ASSERT(ret == 0);

    ret = sys_access(curr_scheduling_proc, "/dev/bar.txt", F_OK);
    ASSERT(ret == ENOENT);

    ret = sys_unlink(curr_scheduling_proc, filename);
    ASSERT(ret == 0);

    fd2 = sys_dup(curr_scheduling_proc, fd);
    ASSERT(fd2 >= 0);



    ret = sys_mknod(curr_scheduling_proc, "/dev/tty", O_RDWR, MAKEDEV(3, 1));
    ASSERT(ret == 0);

    fd3 = sys_open(curr_scheduling_proc, "/dev/tty", O_RDWR, 0);
    ASSERT(fd >= 0);

    ret = sys_read(curr_scheduling_proc, fd3, buffer, 3);
    ASSERT(ret == 3);
    ASSERT(strcmp(buffer, "tt") == 0);

    fd4 = sys_dup2(curr_scheduling_proc, fd3, fd2);
    ASSERT(fd4 == fd2);

    ret = sys_read(curr_scheduling_proc, fd4, buffer, 4);
    ASSERT(ret == 4);
    ASSERT(strcmp(buffer, "ttt") == 0);

    ret = sys_close(curr_scheduling_proc, fd);
    ASSERT(ret == 0);

    ret = sys_close(curr_scheduling_proc, fd2);
    ASSERT(ret == 0);

    ret = sys_close(curr_scheduling_proc, fd3);
    ASSERT(ret == 0);

    ret = sys_close(curr_scheduling_proc, fd4);
    ASSERT(ret == EBADF);

    printf("filesystem unit test passed");
    return 0;
}