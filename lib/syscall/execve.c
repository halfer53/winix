#include <sys/syscall.h>

extern char **_environ;

// int execve(const char *pathname, char *const argv[],
//                   char *const envp[]){
//     int ret;
//     struct message m;
//     m.m1_p1 = (void* )pathname;
//     m.m1_p2 = (void* )argv;
//     m.m1_p3 = (void* )envp;
//     ret = _syscall(EXECVE, &m);
//     return ret;
// }

int execv(const char *path, char *const argv[]){
    init_environ();
    return execve(path, argv, _environ);
}

