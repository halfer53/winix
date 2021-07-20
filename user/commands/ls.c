#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>

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
    int i;
    struct user_name* p = usernames;
    while (p)
    {
        if(p->uid == uid)
            return p->str;
        p++;
    }
    return NULL;
}


void int2str(int value, int i, char* output){
    int j;
    char *p = output;
    while(i){
        j = (value / i) % 10;
        *output++ = '0' + j;
        i /= 10;
    }
    *output = '\0';
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

#define get_group_name(id)  (get_user_name(id))

#define SHOW_HIDDEN     1
#define LONG_FORMAT     2
#define HUMAN_FORMAT    4

static char buffer[128];
static char permission_char[] = "-xwr";

void print_long_format(char *pathname, int flag){
    struct stat statbuf;
    struct time_struct time;
    char size_buf[10];
    char *unit_s;
    char *p = buffer;
    int i, j, k, l;
    int ret;
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
    if(flag & HUMAN_FORMAT){
        set_num_str(statbuf.st_size, size_buf);
        unit_s = "KB";
    }else{
        char *p = size_buf;
        int2str(statbuf.st_size, 10000, size_buf);
        while(*p){
            if(*p == '0'){
                *p = ' ';
            }else{
                break;
            }
            p++;
        }
        unit_s = "";
    }
    username = get_user_name(statbuf.st_uid);
    groupname = get_group_name(statbuf.st_gid);
    parse_unix_time(statbuf.st_atime, &time);
    printf("%s %2d %s %s %s%s %02d/%02d/%04d %02d:%02d:%02d %s\n", buffer, statbuf.st_nlink, username, groupname,
     size_buf, unit_s, time.date, time.month, time.currYear, time.hours, time.minutes, time.seconds, pathname);
}

#define PATH_LEN    (50)

int do_ls(char* pathname, int flag){
   DIR* directory;
   struct dirent* dir;
   int ret;
   char *slash = "/";
   char *symbol;
   char path_buffer[PATH_LEN];
   
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
           strlcpy(path_buffer, pathname, PATH_LEN);
           strlcat(path_buffer, "/", PATH_LEN);
           strlcat(path_buffer, (char *)dir->d_name, PATH_LEN);
           print_long_format(path_buffer, flag);
       }else{
           symbol = (dir->d_type == DT_DIR && *dir->d_name != '.')  ? slash : "";
            printf("%s%ls  ", symbol, dir->d_name);
       }
       
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


