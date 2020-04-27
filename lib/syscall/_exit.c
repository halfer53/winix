#include <sys/syscall.h>

/**
 * Exits the current process.
 **/
 int ___exit(int status) {
    struct message m;
    m.m1_i1 = status;
    // if a process is killed by signals
    // m1_i2 is used for indicate the signal that 
    // killed this process
    // but since the process is exiting normally,
    // its set to 0
    m.m1_i2 = 0;
    return _syscall(EXIT, &m);
}

int exit(int status){
    ___exit(status);
    // should never get here
    return -1;
}


