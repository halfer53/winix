#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define PATH_LEN    (64)
char slash[] = "/";

int count_dir_size(DIR* directory, char *parent_name){
    struct dirent* dir;
    int count = 0;
    char *path = malloc(PATH_LEN);
    strncpy(path, parent_name, PATH_LEN);
    while((dir = readdir(directory))){
        if(*dir->d_name == '.')
            continue;
        if(dir->d_type == DT_DIR){
            
            DIR* subdir = opendir(dir->d_name);
            count += count_dir_size(subdir, parent_name);
        }
    }
}

int main(int argc, char **argv){
    DIR* directory;
    struct dirent* dir;
    char *curr_dir = ".";
    if(argc > 1)
        curr_dir = argv[1];

    directory = opendir(curr_dir);
    if(!directory)
        return -1;
}

