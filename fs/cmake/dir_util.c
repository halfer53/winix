//
// Created by bruce on 3/05/20.
//

#include <dirent.h>
#include <stddef.h>
#include "../fs.h"

int do_ps(char* pathname){
    DIR* directory;
    struct dirent* dir;
    int ret;
    if(pathname == NULL)
        pathname = ".";
    directory = opendir(pathname);
    if(!directory)
        return -1;
    printf("Do PS: ");
    while((dir = readdir(directory)) != NULL){
        printf("%s  ", dir->d_name);
    }
    printf("\n");
    closedir(directory);
    return 0;
}

DIR *opendir(const char *pathname){
    DIR directory, *dir_ptr;
    int fd;
    fd = sys_open(current_proc, pathname, O_RDWR, 0x777);
    if(fd < 0)
        return NULL;
    directory.fd = fd;
    directory.pos = 0;
    dir_ptr = kmalloc(sizeof(DIR));
    if(!dir_ptr)
        return NULL;
    *dir_ptr = *(&directory);
    return dir_ptr;
}

struct dirent *readdir(DIR *directory){
    struct dirent* dir;
    int ret;
    do{
        ret = sys_read(current_proc, directory->fd, directory->buffer, DIRSIZ);
        if(ret <= 0)
            return NULL;
        dir = (struct dirent*)directory->buffer;
    }while(dir->d_ino == 0);
    return dir;
}

int closedir(DIR *directory){
    int ret = sys_close(current_proc, directory->fd);
    kfree(directory);
    return ret;
}



