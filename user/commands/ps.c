#include <sys/syscall.h>
#include <sys/fcntl.h>

int main(int argc, char *argv[]){
    return wramp_syscall(WINFO, WINFO_PS);
}



