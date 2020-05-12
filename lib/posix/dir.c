//
// Created by bruce on 3/05/20.
//
#include <lib.h>

DIR *opendir(const char *pathname){
   DIR *dir_ptr;
   int fd;
   fd = open(pathname, O_RDWR, 0x777);
   if(fd < 0)
       return NULL;
   dir_ptr = malloc(sizeof(DIR));
   if(!dir_ptr)
       return NULL;
   dir_ptr->fd = fd;
   dir_ptr->pos = 0;
   return dir_ptr;
}

struct dirent *readdir(DIR *directory){
   struct dirent* dir;
   int ret;
   ret = getdent(directory->fd, &directory->buffer);
   if(ret <= 0){
    //    printf("readdir errno %d\n", errno);
       return NULL;
   }
       
   dir = &directory->buffer;
   return dir;
}

int closedir(DIR *directory){
   int ret = close(directory->fd);
   free(directory);
   return ret;
}



