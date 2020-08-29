#include <lib.h>

int open(const char *pathname,int flags, ...){
    struct message msg;
    msg.m1_p1 = (void*)pathname;
    msg.m1_i1 = flags;
    msg.m1_i2 = *((int *)&flags+1);
    return _syscall(OPEN, &msg);
}

int creat(const char *pathname, mode_t mode){
    struct message m;
    m.m1_p1 = (void*)pathname;
    m.m1_i1 = mode;
    return _syscall(CREAT, &m);
}

// int close(int fd){
//     struct message m;
//     m.m1_i1 = fd;
//     return _syscall(CLOSE, &m);
// }

// size_t read(int fd, void *buf, size_t count){
//     struct message m;
//     m.m1_i1 = fd;
//     m.m1_p1 = (void*)buf;
//     m.m1_i2 = count;
//     return _syscall(READ, &m);
// }

// size_t write(int fd, const void *buf, size_t count){
//     struct message m;
//     m.m1_i1 = fd;
//     m.m1_p1 = (void*)buf;
//     m.m1_i2 = count;
//     return _syscall(WRITE, &m);
// }

// int pipe(int pipefd[2]){
//     struct message m;
//     m.m1_p1 = (void*)pipefd;
//     return _syscall(PIPE, &m);
// }

// int mknod(const char *pathname, mode_t mode, dev_t dev){
//     struct message m;
//     m.m1_p1 = (void*)pathname;
//     m.m1_i1 = mode;
//     m.m1_i2 = dev;
//     return _syscall(MKNOD, &m);
// }

// int chdir(const char *path){
//     struct message m;
//     m.m1_p1 = (void*)path;
//     return _syscall(CHDIR, &m);
// }

// int chown(const char *pathname, uid_t owner, gid_t group){
//     struct message m;
//     m.m1_p1 = (void*)pathname;
//     m.m1_i1 = owner;
//     m.m1_i2 = group;
//     return _syscall(CHOWN, &m);
// }

// int chmod(const char *pathname, mode_t mode){
//     struct message m;
//     m.m1_p1 = (void*)pathname;
//     m.m1_i1 = mode;
//     return _syscall(CHMOD, &m);
// }

int stat(const char *pathname, struct stat *statbuf){
    struct message m;
    m.m1_p1 = (void*)pathname;
    m.m1_p2 = (void*)statbuf;
    return _syscall(STAT, &m);
}

int fstat(int fd, struct stat *statbuf){
    struct message m;
    m.m1_i1 = fd;
    m.m1_p1 = (void*)statbuf;
    return _syscall(FSTAT, &m);
}

int dup(int oldfd){
    struct message m;
    m.m1_i1 = oldfd;
    return _syscall(DUP, &m);
}

int dup2(int oldfd, int newfd){
    struct message m;
    m.m1_i1 = oldfd;
    m.m1_i2 = newfd;
    return _syscall(DUP2, &m);
}

int link(const char *oldpath, const char *newpath){
    struct message m;
    m.m1_p1 = (void*)oldpath;
    m.m1_p2 = (void*)newpath;
    return _syscall(LINK, &m);
}

int unlink(const char *pathname){
    struct message m;
    m.m1_p1 = (void*)pathname;
    return _syscall(UNLINK, &m);
}



int access(const char *pathname, int mode){
    struct message m;
    m.m1_p1 = (void*)pathname;
    m.m1_i1 = mode;
    return _syscall(ACCESS, &m);
}

int mkdir(const char *pathname, mode_t mode){
    struct message m;
    m.m1_p1 = (void*)pathname;
    m.m1_i1 = mode;
    return _syscall(MKDIR, &m);
}

// int getdents( int fd, struct dirent *dirp, unsigned int count){
//     struct message m;
//     m.m1_i1 = fd;
//     m.m1_i2 = count;
//     m.m1_p1 = (void*)dirp;
//     return _syscall(GETDENT, &m);
// }

// int sync(){
//     struct message m;
//     return _syscall(SYNC, &m);
// }

off_t lseek(int fd, off_t offset, int whence){
    struct message m;
    m.m1_i1 = fd;
    m.m1_i2 = offset;
    m.m1_i3 = whence;
    return (off_t)_syscall(LSEEK, &m);
}

mode_t umask(mode_t mask){
    struct message m;
    m.m1_i1 = mask;
    return (mode_t)_syscall(UMASK, &m);
}

int ioctl(int fd, unsigned long request, ...){
    struct message m;
    m.m1_i1 = fd;
    m.m1_i2 = request;
    m.m1_p1 = ((int *)&request) + 1;
    return _syscall(IOCTL, &m);
}

