 #include <lib.h>

void abort(){
    kill(getpid(),SIGABRT);
}

