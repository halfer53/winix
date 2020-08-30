#include <stdio.h>
#include <dirent.h>

#define SHOW_HIDDEN     1
#define LONG_FORMAT     2

static char buffer[128];

void print_long_format(char *pathname){
    struct stat statbuf;
    char *p;
    int i = 800;
}


int do_ls(char* pathname, int flag){
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
       if(*dir->d_name == '.' && !(flag & SHOW_HIDDEN)){
           continue;
       }
       symbol = (dir->d_type == DT_DIR && *dir->d_name != '.')  ? slash : "";
        printf("%s%ls  ", symbol, dir->d_name);
   }
   closedir(directory);
   return 0;
}

void usage(){
    fprintf(stderr, "ls [-l] [-a] FOLDER\n");
    exit(1);
}

int main(int argc, char*argv[]){
    char* path = ".";
    int ret, flag = 0, i = 0;
    int k;
    char *cp;
    int fd;

    /* Get flags. */
    k = 1;
    cp = argv[1];

    if (cp && *cp++ == '-')
    {
        k++; /* points to first file */
        while (*cp != 0)
        {
            switch (*cp)
            {
            case 'a':
                flag |= SHOW_HIDDEN;
                break;
            case 'l':
                flag |= LONG_FORMAT;
                break;
            default:
                usage();
            }
            cp++;
        }
    }

    if(k < argc){
        path = argv[k];
    }
    
    ret = do_ls(path, flag);
    // printf("ls opened %d %s\n", ret, path);
    return ret;
}


