#include <sys/syscall.h>

int main(int argc, char **argv){
    struct statfs buf;
    int ret = statfs("/", &buf);
    if(ret)
        return ret;
    printf("\nFileSystem status:\n%d blocks in total, %d blocks free\n%d inodes in total, %d inodes free. \n",
             buf.f_blocks, buf.f_bfree, buf.f_files, buf.f_ffree);
    return 0;
}


