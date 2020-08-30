#include <sys/syscall.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>

#define BUFFER_SIZ  256

static char buffer[BUFFER_SIZ];

int main(int argc, char *argv[]){
    int fd, ret, dest_fd;
    struct stat statbuf, dest_statbuf;
    char *path, *dest;

    if(argc < 3){
        fprintf(stderr, "mv SRC DEST");
        goto fail;
    }
    path = *++argv;
    dest = *++argv;
    ret = stat(path, &statbuf);
    if(ret){
        perror(path);
        goto fail;
    }

    ret = stat(dest, &dest_statbuf);
    if(ret == 0){
        if(!S_ISREG(dest_statbuf.st_mode)){
            fprintf(stderr, "dest not regular file\n");
            goto fail;
        }
        while(1){
            printf("overwrite %s (y/n)? ", dest);
            ret = read(STDIN_FILENO, buffer, BUFFER_SIZ);
            if(buffer[0] == 'n'){
                return 0;
            }else if(buffer[0] == 'y'){
                ret = unlink(dest);
                if(ret){
                    perror("cant overwrite");
                    goto fail;
                }
                break;
            }else{
                printf("plz type y or n\n");
            }
        }
    }
    ret = link(path, dest);
    if(ret){
        perror("cant link");
        goto fail;
    }

    ret = unlink(path);
    if(ret){
        perror("unlink");
        goto fail;
    }
    return 0;

fail:
    return 1;

}

