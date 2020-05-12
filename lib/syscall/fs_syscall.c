#include <lib.h>

int open(const char *pathname,int flags, mode_t mode){
    struct message msg;
    msg.m1_p1 = (void*)pathname;
    msg.m1_i1 = flags;
    msg.m1_i2 = mode;
    return _syscall(OPEN, &msg);
}

int creat(const char *pathname, mode_t mode){
    struct message m;
    m.m1_p1 = (void*)pathname;
    m.m1_i1 = mode;
    return _syscall(CREAT, &m);
}

int close(int fd){
    struct message m;
    m.m1_i1 = fd;
    return _syscall(CLOSE, &m);
}

size_t read(int fd, void *buf, size_t count){
    struct message m;
    m.m1_i1 = fd;
    m.m1_p1 = (void*)buf;
    m.m1_i2 = count;
    return _syscall(READ, &m);
}

size_t write(int fd, const void *buf, size_t count){
    struct message m;
    m.m1_i1 = fd;
    m.m1_p1 = (void*)buf;
    m.m1_i2 = count;
    return _syscall(WRITE, &m);
}

int pipe(int pipefd[2]){
    struct message m;
    m.m1_p1 = (void*)pipefd;
    return _syscall(PIPE, &m);
}

int access(const char *pathname, int mode){
    struct message m;
    m.m1_p1 = (void*)pathname;
    m.m1_i1 = mode;
    return _syscall(ACCESS, &m);
}

int chdir(const char *path){
    struct message m;
    m.m1_p1 = (void*)path;
    return _syscall(CHDIR, &m);
}

int mkdir(const char *pathname, mode_t mode){
    struct message m;
    m.m1_p1 = (void*)pathname;
    m.m1_i1 = mode;
    return _syscall(MKDIR, &m);
}

int getdent( int fd, struct dirent *dirp){
    struct message m;
    m.m1_i1 = fd;
    m.m1_p1 = (void*)dirp;
    return _syscall(GETDENT, &m);
}