#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

struct time_struct{
    int date;
    int month;
    int currYear;
    int hours;
    int minutes;
    int seconds;
};

// Function to convert unix time to
// Human readable format
int parse_unix_time(unsigned int seconds, struct time_struct* time)
{ 
    // Number of days in month
    // in normal year
    int daysOfMonth[] = { 31, 28, 31, 30, 31, 30,
                          31, 31, 30, 31, 30, 31 };
 
    int currYear, daysTillNow, extraTime,
        extraDays, index, date, month, hours,
        minutes, secondss, flag = 0;
 
    // Calculate total days unix time T
    daysTillNow = seconds / (24 * 60 * 60);
    extraTime = seconds % (24 * 60 * 60);
    currYear = 1970;
 
    // Calculating currrent year
    while (daysTillNow >= 365) {
        if (currYear % 400 == 0
            || (currYear % 4 == 0
                && currYear % 100 != 0)) {
            daysTillNow -= 366;
        }
        else {
            daysTillNow -= 365;
        }
        currYear += 1;
    }
 
    // Updating extradays because it
    // will give days till previous day
    // and we have include current day
    extraDays = daysTillNow + 1;
 
    if (currYear % 400 == 0
        || (currYear % 4 == 0
            && currYear % 100 != 0))
        flag = 1;
 
    // Calculating MONTH and DATE
    month = 0, index = 0;
    if (flag == 1) {
        while (true) {
 
            if (index == 1) {
                if (extraDays - 29 < 0)
                    break;
                month += 1;
                extraDays -= 29;
            }
            else {
                if (extraDays
                        - daysOfMonth[index]
                    < 0) {
                    break;
                }
                month += 1;
                extraDays -= daysOfMonth[index];
            }
            index += 1;
        }
    }
    else {
        while (true) {
 
            if (extraDays
                    - daysOfMonth[index]
                < 0) {
                break;
            }
            month += 1;
            extraDays -= daysOfMonth[index];
            index += 1;
        }
    }
 
    // Current Month
    if (extraDays > 0) {
        month += 1;
        date = extraDays;
    }
    else {
        if (month == 2 && flag == 1)
            date = 29;
        else {
            date = daysOfMonth[month - 1];
        }
    }
 
    // Calculating HH:MM:YYYY
    hours = extraTime / 3600;
    minutes = (extraTime % 3600) / 60;
    secondss = (extraTime % 3600) % 60;

    time->date = date;
    time->month = month;
    time->currYear = currYear;
    time->hours = hours;
    time->minutes = minutes;
    time->seconds = secondss;
 
    return 0;
}


struct user_name{
    uid_t uid;
    char *str;
};


// currently user names are predefined cuz I am lazy
struct user_name usernames[] = {
    {0, "root"},
    {0}
};

char *get_user_name(uid_t uid){
    struct user_name* p = usernames;
    while (p)
    {
        if(p->uid == uid)
            return p->str;
        p++;
    }
    return NULL;
}

#define get_group_name(id)  (get_user_name(id))

#define SHOW_HIDDEN     1
#define LONG_FORMAT     2
#define HUMAN_FORMAT    4

static char buffer[128];
static char permission_char[] = "-xwr";

void print_long_format(char *pathname, int flag){
    struct stat statbuf;
    struct time_struct time;
    char *unit_s;
    char *p = buffer;
    int i, j, k;
    int ret;
    off_t size;
    mode_t mode;
    char *username, *groupname;
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

    username = get_user_name(statbuf.st_uid);
    groupname = get_group_name(statbuf.st_gid);
    printf("%s %2d %s %s ", buffer, statbuf.st_nlink, username, groupname);

    size = statbuf.st_size;
    if(flag & HUMAN_FORMAT){
        off_t rem = size % 1024;
        rem = rem >= 1000 ? 999 : rem;
        printf("%3d.%.3d ", size / 1024, rem);
        unit_s = "KB";
    }else{
        printf("%5d ", size);
        unit_s = "";
    }
    
    parse_unix_time(statbuf.st_atime, &time);
    printf("%s %02d/%02d/%04d %02d:%02d:%02d %s\n", unit_s, time.date, time.month, time.currYear,
         time.hours, time.minutes, time.seconds, pathname);
}

#define PATH_LEN    (50)

int do_ls(char* pathname, int flag){
   DIR* directory;
   struct dirent* dir;
   char *slash = "/";
   char *symbol;
   char path_buffer[PATH_LEN];
   
   if(pathname == NULL)
       pathname = ".";
   directory = opendir(pathname);
   if(!directory){
       perror("opendir");
       return 1;
   }
       
   while((dir = readdir(directory)) != NULL){
       if(*dir->d_name == '.' && !(flag & SHOW_HIDDEN)){
           continue;
       }
       if(flag & LONG_FORMAT){
           strlcpy(path_buffer, pathname, PATH_LEN);
           strlcat(path_buffer, "/", PATH_LEN);
           strlcat(path_buffer, (char *)dir->d_name, PATH_LEN);
           print_long_format(path_buffer, flag);
       }else{
            symbol = (dir->d_type == DT_DIR && *dir->d_name != '.')  ? slash : "";
            printf("%s%s  ", symbol, (char *)dir->d_name);
       }
   }
   if (!(flag & LONG_FORMAT)){
       printf("\n");
   }
   closedir(directory);
   return 0;
}

void usage(){
    fprintf(stderr, "ls [-l] [-a] [-h] FOLDER\n");
    exit(1);
}

int main(int argc, char*argv[]){
    char* path = ".";
    int ret, flag = 0;
    int k;
    char *cp;

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
            case 'h':
                flag |= HUMAN_FORMAT;
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


