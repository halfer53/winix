//
// Created by bruce on 3/05/20.
//
#include <lib.h>

DIR *opendir(const char *pathname){
   DIR *dir_ptr;
   int fd;
   fd = open(pathname, O_RDWR);
   if(fd < 0)
       return NULL;
   dir_ptr = malloc(sizeof(DIR));
   if(!dir_ptr)
       return NULL;
   dir_ptr->fd = fd;
   dir_ptr->pos = 0;
   dir_ptr->limit = 0;
   return dir_ptr;
}

struct dirent *readdir(DIR *directory){
   struct dirent* dir;
   int ret;

   if(directory->limit){
      if(directory->pos < directory->limit)
         return &directory->buffer[directory->pos++];
      if(directory->limit < DIR_BUFFER_LEN)
         return NULL;
   }
   ret = getdents(directory->fd, directory->buffer, DIR_BUFFER_LEN);
   if(ret <= 0){
      // perror("readdir");
      return NULL;
   }
   directory->pos = 1;
   directory->limit = ret / sizeof(struct dirent);
   return directory->buffer;
}

int closedir(DIR *directory){
   int ret = close(directory->fd);
   free(directory);
   return ret;
}



