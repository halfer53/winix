#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define PATH_LEN    (64)
char slash[] = "/";

void reset_path(char *dir_path, char *path){
    strlcpy(path, dir_path, PATH_LEN);
    strlcat(path, slash, PATH_LEN);
}


void int2str(int value, int i, char* output){
    int j;
    while(i){
        j = (value / i) % 10;
        *output++ = '0' + j;
        i /= 10;
    }
}

void set_num_str(int value, char *buf){
    int size, mod;
    size = value / 4096;
    mod = value % 4096;
    int2str(size, 10, buf);
    if(*buf == '0'){
        *buf = ' ';
    }
    buf += 2;
    *buf++ = '.';
    int2str(mod, 10, buf);
    buf += 2;
    *buf = '\0';
}

size_t count_dir_size(char *dir_path){
    struct dirent* dir;
    struct stat statbuf;
    size_t count = 0, ret;
    char *path;
    char size_buf[10];
    DIR* directory = opendir(dir_path);
    if(!directory){
        fprintf(stderr, "err:%s\n", dir_path);
        return 0;
    }
    path = malloc(PATH_LEN);
    reset_path(dir_path, path);
    // printf("opening %s fd %d\n", dir_path, directory->fd);
    while((dir = readdir(directory)) != NULL){
        if(*dir->d_name == '.')
            continue;
        strlcat(path, (const char*)dir->d_name, PATH_LEN);
        if(dir->d_type == DT_DIR){
            count += count_dir_size(path);
        }else{
            ret = stat(path, &statbuf);
            // printf("stat %s %d size %d\n", dir->d_name, ret, statbuf.st_size);
            count += (size_t)statbuf.st_size;
        }
        reset_path(dir_path, path);
    }
    set_num_str(count, size_buf);
    printf("%sKB %s\n", size_buf, dir_path);
    closedir(directory);
    free(path);
    return count;
}

int main(int argc, char **argv){
    char *curr_dir = ".";
    if(argc > 1)
        curr_dir = argv[1];

    count_dir_size(curr_dir);
    return 0;
}

