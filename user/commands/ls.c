#include <stdio.h>
#include <unistd.h>

int do_ls(char* pathname){
   DIR* directory;
   struct dirent* dir;
   int ret;
   char *slash = "/";
   char *symbol;
   if(pathname == NULL)
       pathname = ".";
   directory = opendir(pathname);
   if(!directory)
       return -1;
   while((dir = readdir(directory)) != NULL){
       if(*dir->d_name == '.'){
           continue;
       }
       symbol = dir->d_type == DT_DIR ? slash : "";
       printf("%s%ls  ", symbol, dir->d_name);
   }
   printf("\n");
   closedir(directory);
   return 0;
}

int main(int argc, char*argv[]){
    char* path = ".";
    int ret;
    if(argc == 2)
        path = argv[1];
    
    ret = do_ls(path);
    // printf("ls opened %d %s\n", ret, path);
    return ret;
}


