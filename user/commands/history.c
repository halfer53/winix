#include <sys/syscall.h>
#include <sys/fcntl.h>

static char filename[] = "/.bash_history";
#define SIZE    128
static char buffer[SIZE];
int main(int argc, char *argv[]){
    int fd, ret;
    fd = open(filename, O_WRONLY);
    if(fd < 0){
        perror(filename);
        return 1;
    }
    while((ret = read(fd, buffer, SIZE)) > 0){
        write(STDOUT_FILENO, buffer, ret);
    }
    close(fd);
    return 0;
}


