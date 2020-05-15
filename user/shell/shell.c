/**
 * 
 * Simple shell for WINIX.
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/

#include "shell.h"

// Input buffer & tokeniser
static char buf[MAX_LINE];

// Prototypes
CMD_PROTOTYPE(ps);
CMD_PROTOTYPE(uptime);
CMD_PROTOTYPE(cmd_kill);
CMD_PROTOTYPE(test_general);
CMD_PROTOTYPE(print_pid);
CMD_PROTOTYPE(mem_info);
CMD_PROTOTYPE(mall_info);
CMD_PROTOTYPE(generic);
CMD_PROTOTYPE(help);
CMD_PROTOTYPE(cmd_exit);
CMD_PROTOTYPE(printenv);
CMD_PROTOTYPE(cmd_bash);
CMD_PROTOTYPE(do_cd);


// Command handling
struct cmd_internal builtin_commands[] = {
    { test_general, "test"},
    { printenv, "printenv" },
    { mall_info, "printheap"},
    { cmd_bash, "bash"},
    { uptime, "uptime"},
    { ps, "ps"},
    { cmd_kill, "kill"},
    { print_pid, "pid"},
    { mem_info, "free"},
    { cmd_exit, "exit"},
    { help, "help"},
    { do_cd, "cd"},
    { generic, NULL }
};

void testfoo(){
    int n = 100;
    int m = 32;
}

void init_shell(){
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
}


int main() {
    int read_nr;
    char ret;
    char *c;
    char *end_buf;

    init_shell();

    // test_malloc(0, NULL);
    c = buf;
    end_buf = c + MAX_LINE -2;
    while(1) {
        printf("WINIX> ");
        c = buf;
        // Read line from terminal
        while( c < end_buf) {
            // ret = getchar();     // read
            
            read_nr = read(0, &ret, 1);
            // printf("ret %d err %d addr %p val %d\n",read_nr, errno, &ret, ret);

            if(ret == EOF){
                if(errno == EINTR){
                    perror("getc(): ");
                    printf("WINIX> ");
                }
                continue;
            }
            
            if(ret == '\r'){// test for end
                break;
            }
                    

            if ((int)ret == 8) { // backspace
                if(c > buf){
                    putchar(ret);
                    c--;
                }
                continue;
            }
            
            if(isprint(ret)){
                *c++ = ret;
                putchar(ret);         // echo
            }else{
                putchar(7);            // beep
            }
            
        }
        
        *c = '\0';
        putchar('\n');
        
        exec_cmd(buf, NULL);     
    }
    return 0;
}



int exec_cmd(char *line, int tpipe[2]){
    struct cmdLine cmd;
    int ret;
    char *p;
    char* buf;
    struct cmd_internal *handler = NULL;

    ret = parse(line,&cmd);

    if(cmd.env && cmd.env_val){ // if a new environment variable is set
        buf = malloc(MAX_LINE);
        parse_quotes(cmd.env_val, buf);
        
        if(*buf){
            printf("setenv %s=%s\n", cmd.env, buf);
            setenv(cmd.env, buf, 1);
        }
        free(buf);
        return 0;
    }

    if(!cmd.argv[0])
        return generic(cmd.argc, cmd.argv);

    // Decode command
    handler = builtin_commands;
    while(handler->name != NULL && strcmp(cmd.argv[0], handler->name)) {
        handler++;
    }
    // Run it
    return handler->handle(cmd.argc, cmd.argv);
}


int printenv(int argc, char **argv){
    const char **v;
    const char *p;
    v = environ;
    while((p = *v++)){
        printf("%s\n",p);
    }
    return 0;
}

int cmd_kill(int argc, char **argv){
    pid_t pid;
    int signum = SIGTERM;
    if(argc < 2){
        printf("kill [-s signum] pid\n");
        return -1;
    }
        
    if(strcmp("-s",argv[1]) == 0){
        signum = atoi(argv[2]);
        pid = atoi(argv[3]);
    }else{
        pid = atoi(argv[1]);
    }

    if(kill(pid,signum))
        perror("kill ");
    return 0;
}

int do_cd(int argc, char** argv){
    int ret;
    if(argc < 2)
        return -1;
    ret = chdir(argv[1]);
    return ret;
}

int help(int argc, char** argv){
    struct cmd_internal* handler;
    handler = builtin_commands;
    printf("Available Commands\n");
    while(handler->name != NULL) {
        printf(" * %s\n",handler->name);
        handler++;
    }
    test_nohandler(0, NULL);
    return 0;
}

/**
 * Prints the system uptime
 **/
 int uptime(int argc, char **argv) {
    
    int ticks, days, hours, minutes, seconds, tick_rate;
    struct tms tbuf;
    ticks = times(&tbuf);
    tick_rate = sysconf(_SC_CLK_TCK);
    seconds = ticks / tick_rate; 
    minutes = seconds / 60;
    hours = minutes / 60;
    days = hours / 24;

    seconds %= 60;
    minutes %= 60;
    hours %= 24;

    printf("Uptime is %dd %dh %dm %d.%02ds\n", days, hours, minutes, seconds, ticks % tick_rate);
    printf("user time %d.%d seconds, system time %d.%02d seconds\n",
                            tbuf.tms_utime  / tick_rate , (tbuf.tms_utime) % tick_rate,
                            tbuf.tms_stime / tick_rate , (tbuf.tms_stime) % tick_rate);
    return 0;
}

// Print the user space heap
int mall_info(int argc, char** argv){
    print_heap();
    return 0;
}

// Print the system wise memory info
int mem_info(int argc, char** argv){
    return sys_meminfo();
}

// current pid
int print_pid(int argc, char **argv){
    printf("%d\n",getpid());
    return 0;
}

// list all the processes in the system
int ps(int argc, char **argv){
    return sys_ps();
}

// start a new bash shell, parent shell is blocked until child shell exits
int cmd_bash(int argc, char **argv){
    pid_t child_pid;
    if(child_pid = fork()){
        if(child_pid == -1){
            perror("fork failed");
            return -1;
        }
        // printf("parent shell %d waiting for child shell %d\n",getpid(),child_pid);
        child_pid = wait(NULL);
        // printf("parent shell %d awakened by child shell %d\n",getpid(),child_pid);
    }else{
        printf("Child shell %d [parent %d] start:\n",getpid(),getppid());
    }
    return 0;
}

int cmd_exit(int argc, char **argv){
    int status = 0;
    if(argc > 1)
        status = atoi(argv[1]);
    printf("Bye!\n");
    // printf("Child %d [parent %d] exits\n",getpid(),getppid());
    return exit(status);
}


/**
 * Handles any unknown command.
 **/
int generic(int argc, char **argv) {
    char path[DIRSIZ];
    int ret;
    pid_t pid;
    sigset_t sigmask = 0;
    if(argc == 0)
        return -1;
    strcpy(path, "/bin/");
    strcat(path, argv[0]);
    ret = access(path, F_OK);
    if(ret == 0){
        pid = vfork();
        if(pid == 0){
            sigprocmask(SIG_SETMASK, &sigmask, NULL);
            execv(path, argv);
        }
        ret = wait(NULL);
        // printf("parent awaken\n");
        return 0;
    }

    printf("Unknown command '%s'\r\n", argv[0]);
    return -1;
}



