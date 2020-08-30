#include <lib.h>

// pid_t waitpid(pid_t pid, int *wstatus, int options){
//     struct message m;
//     int ret;
//     m.m1_i1 = pid;
//     m.m1_i2 = options;
//     ret = _syscall(WAITPID, &m);
//     if(wstatus)
//         *wstatus = m.m1_i2;
//     return (pid_t)ret;
// }

// pid_t wait(int *wstatus){
//     return waitpid(-1, wstatus, 0);
// }

//  clock_t times(struct tms* buf) {
//     struct message m;
//     _syscall(TIMES, &m);
//     if(buf){
//         buf->tms_utime = m.m2_l2;
//         buf->tms_stime = m.m2_l3;
//     }
//     return m.m2_l1;
// }

// long sysconf(int name){
//     struct message m;
//     m.m1_i1 = name;
//     return _syscall(SYSCONF, &m);
// }

// int sigsuspend(const sigset_t *mask){
//     struct message m;
//     if(mask == NULL){
//         __set_errno(EFAULT);
//         return -1;
//     }
//     m.m1_i1 = *mask;
//     return _syscall(SIGSUSPEND, &m);
// }

// int sigprocmask(int how, const sigset_t *set, sigset_t *oldset){
//     struct message m;
//     int ret;
//     if(set == NULL){
//         __set_errno(EFAULT);
//         return -1;
//     }
//     m.m1_i1 = how;
//     m.m1_i2 = *set;
//     ret = _syscall(SIGPROCMASK, &m);
//     if(oldset)
//         *oldset = m.m1_i1;
//     return ret;
// }

// int sigpending(sigset_t *set){
//     struct message m;
//     int ret;
//     ret = _syscall(SIGPENDING, &m);
//     if(set)
//         *set = m.m1_i1;
//     return ret;
// }

// int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact){
//     struct message m;
//     m.m1_i1 = signum;
//     m.m1_p1 = (char *)act;
//     m.m1_p2 = (char *)oldact;
//     m.m1_p3 = get_sigreturn_func_ptr();
//     return _syscall(SIGACTION, &m);
// }

// int setpgid(pid_t pid, pid_t pgid){
//     struct message m;
//     m.m1_i1 = pid;
//     m.m1_i2 = pgid;
//     return _syscall(SETPGID, &m);
// }

// pid_t setsid(void){
//     struct message m;
//     return _syscall(SETSID, &m);
// }

// int kill (pid_t pid, int sig){
//     struct message m;
//     m.m1_i1 = pid; 
//     m.m1_i2 = sig;
//     return _syscall(KILL, &m);
// }

// pid_t getpid(){
//     struct message m;
//     _pid = _syscall(GETPID, &m);
//     return _pid;
// }


// int creat(const char *pathname, mode_t mode){
//     struct message m;
//     m.m1_p1 = (void*)pathname;
//     m.m1_i1 = mode;
//     return _syscall(CREAT, &m);
// }

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

// int stat(const char *pathname, struct stat *statbuf){
//     struct message m;
//     m.m1_p1 = (void*)pathname;
//     m.m1_p2 = (void*)statbuf;
//     return _syscall(STAT, &m);
// }

// int fstat(int fd, struct stat *statbuf){
//     struct message m;
//     m.m1_i1 = fd;
//     m.m1_p1 = (void*)statbuf;
//     return _syscall(FSTAT, &m);
// }

// int dup(int oldfd){
//     struct message m;
//     m.m1_i1 = oldfd;
//     return _syscall(DUP, &m);
// }

// int dup2(int oldfd, int newfd){
//     struct message m;
//     m.m1_i1 = oldfd;
//     m.m1_i2 = newfd;
//     return _syscall(DUP2, &m);
// }

// int link(const char *oldpath, const char *newpath){
//     struct message m;
//     m.m1_p1 = (void*)oldpath;
//     m.m1_p2 = (void*)newpath;
//     return _syscall(LINK, &m);
// }

// int unlink(const char *pathname){
//     struct message m;
//     m.m1_p1 = (void*)pathname;
//     return _syscall(UNLINK, &m);
// }



// int access(const char *pathname, int mode){
//     struct message m;
//     m.m1_p1 = (void*)pathname;
//     m.m1_i1 = mode;
//     return _syscall(ACCESS, &m);
// }

// int mkdir(const char *pathname, mode_t mode){
//     struct message m;
//     m.m1_p1 = (void*)pathname;
//     m.m1_i1 = mode;
//     return _syscall(MKDIR, &m);
// }

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

// off_t lseek(int fd, off_t offset, int whence){
//     struct message m;
//     m.m1_i1 = fd;
//     m.m1_i2 = offset;
//     m.m1_i3 = whence;
//     return (off_t)_syscall(LSEEK, &m);
// }

// mode_t umask(mode_t mask){
//     struct message m;
//     m.m1_i1 = mask;
//     return (mode_t)_syscall(UMASK, &m);
// }

// pid_t fork(){
//     int result;
//     struct message m;
//     result = _syscall(FORK, &m);
//     if(result == 0)
//         _pid = 0; // reset pid cache if its child
//     return result;
// }

// int csleep(int ticks){
//     struct message m;
//     m.m1_i1 = ticks;
//     return _syscall(CSLEEP, &m);
// }

// unsigned int alarm(unsigned int seconds){
//     struct message m;
//     if(seconds == 0)
//         return 0;
//     m.m1_i1 = seconds;
//     return _syscall(ALARM, &m);
// }

// pid_t getpgid(pid_t pid){
//     struct message m;
//     m.m1_i1 = pid;
//     _syscall(GETPGID, &m);
//     return m.m1_i1;
// }

// void* get_sigreturn_func_ptr(void);

// sighandler_t signal(int signum, sighandler_t handler){
//     struct sigaction sa, oldsa;
//     sa.sa_handler = handler;
//     sa.sa_flags = SA_RESETHAND;
//     sa.sa_mask = 0xffff;
//     if(sigaction(signum, &sa, &oldsa))
//         return SIG_ERR;
//     return oldsa.sa_handler;
// }

// int fcntl(int fd, int cmd, ... /* arg */ ){
//     struct message m;
//     m.m1_i1 = fd;
//     m.m1_i2 = cmd;
//     m.m1_p1 = (int*)&cmd + 1;
//     return _syscall(FCNTL, &m);
// }

// int open(const char *pathname,int flags, ...){
//     struct message msg;
//     msg.m1_p1 = (void*)pathname;
//     msg.m1_i1 = flags;
//     msg.m1_i2 = *((int *)&flags+1);
//     return _syscall(OPEN, &msg);
// }


// int ioctl(int fd, unsigned long request, ...){
//     struct message m;
//     m.m1_i1 = fd;
//     m.m1_i2 = request;
//     m.m1_p1 = ((int *)&request) + 1;
//     return _syscall(IOCTL, &m);
// }

// static char* _brk = NULL; // data segment break cache

// int brk(void *addr){
//     struct message m;
//     int ret = 0;
//     m.m1_p1 = addr;
//     ret = _syscall(BRK, &m);
//     _brk = m.m1_p1;
//     if(ret < 0)
//         return -1;
//     return ret;
// }


// void *sbrk(int incr){
//     char *newsize, *oldsize;

//     if(_brk == NULL){
//         brk((void *)0xffffffff); // initialise _brk
//     }

//     oldsize = _brk;
//     newsize = _brk + incr;
//     if ((incr > 0 && newsize < oldsize) || (incr < 0 && newsize > oldsize))
//         goto return_err;
    
//     if(incr == 0)
//         return _brk;
    
//     if (brk(newsize) == 0)
//         return(oldsize);

//     return_err:
//         return NULL;
        
// }


