 #include <lib.h>

int abort(){
    return kill(getpid(),SIGABRT);
}

