#include <stdio.h>
#include <dirent.h>

#define SHOW_HIDDEN     1
#define LONG_FORMAT     2

static char buffer[128];
static char permission_char[] = "-xwr";

void print_long_format(char *pathname){
    struct stat statbuf;
    char *p = buffer;
    int i, j, k, l;
    int ret;
    mode_t mode;
    ret = stat(pathname, &statbuf);
    if(ret)
        return;
    mode = statbuf.st_mode;
    *p++ = S_ISDIR(statbuf.st_mode) ? 'd' : '-';
    k = 0x400;
    for(i = 3; i > 0; i--){ 
        for(j = 3; j > 0; j--){
            *p++ = k & mode ? permission_char[j] : '-';
            // printf("k %x mode %x\n", k, mode);
            k = k >> 1;
        }
        k = k >> 1;
    }
    *p = '\0';
    printf("%s %2d %4d %s\n", buffer, statbuf.st_nlink, statbuf.st_size, pathname);
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
       return 1;
   while((dir = readdir(directory)) != NULL){
       if(*dir->d_name == '.' && !(flag & SHOW_HIDDEN)){
           continue;
       }
       if(flag & LONG_FORMAT){
           print_long_format((char *)dir->d_name);
       }else{
           symbol = (dir->d_type == DT_DIR && *dir->d_name != '.')  ? slash : "";
            printf("%s%ls  ", symbol, dir->d_name);
       }
       
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


