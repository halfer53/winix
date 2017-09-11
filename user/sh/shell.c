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


//Input buffer & tokeniser
static char buf[MAX_LINE];

//Prototypes
CMD_PROTOTYPE(ps);
CMD_PROTOTYPE(uptime);
CMD_PROTOTYPE(cmd_kill);
CMD_PROTOTYPE(test_general);
CMD_PROTOTYPE(print_pid);
CMD_PROTOTYPE(mem_info);
CMD_PROTOTYPE(mall_info);
CMD_PROTOTYPE(generic);
CMD_PROTOTYPE(help);
CMD_PROTOTYPE(printenv);

CMD_PROTOTYPE(test_malloc);
CMD_PROTOTYPE(test_so);
CMD_PROTOTYPE(test_float);
CMD_PROTOTYPE(test_fork);
CMD_PROTOTYPE(test_thread);
CMD_PROTOTYPE(test_alarm);
CMD_PROTOTYPE(test_signal);
CMD_PROTOTYPE(test_nohandler);

//Command handling
struct cmd builtin_commands[] = {
    { test_general, "test"},
    { uptime, "uptime" },
    { printenv, "printenv" },
    { ps, "ps" },
    { cmd_kill, "kill"},
    { print_pid, "pid"},
    { mem_info, "free"},
    { mall_info, "mallinfo"},
    { help, "?"},
    { generic, NULL }
};

struct cmd test_commands[] = {
    { test_malloc, "malloc"}, 
    { test_so, "stack"},
    { test_float, "float"},
    { test_fork, "fork"},
    { test_thread, "thread"},
    { test_alarm, "alarm"},
    { test_signal, "signal"},
    { test_nohandler, NULL},
};


int test_nohandler(int argc, char** argv){
    int i;
    struct cmd* handler;
    handler = test_commands;
    printf("Available Test Options\n");
    for( i = 0; i < sizeof(test_commands) / sizeof(struct cmd) - 1; i++){
        if(handler->name)
            printf(" * %s\n",handler->name);
        handler++;
    }
    printf("e.g. \"test thread 100\"\n");
    return 0;
}

int test_general(int argc, char **argv){
    struct cmd* handler;
    handler = test_commands;
    while(handler->name != NULL && strcmp(argv[1], handler->name)) {
        handler++;
    }
    //Run it
    handler->handle(argc-1, argv+1);
    return 0;
}

extern const char** _penviron;
int printenv(int argc, char **argv){
    const char **v;
    const char *p;
    if(_penviron == NULL)
        init_environ();
    v = _penviron;
    while((p = *v++)){
        printf("%s\n",p);
    }
    return 0;
}

int cmd_kill(int argc, char **argv){
    pid_t pid;
    int signum = SIGKILL;
    if(argc < 2){
        printf("kill [-n signum] pid\n");
        return -1;
    }
        
    if(strcmp("-n",argv[1]) == 0){
        signum = atoi(argv[2]);
        pid = atoi(argv[3]);
    }else{
        pid = atoi(argv[1]);
    }
    return kill(pid,signum);
}

int help(int argc, char** argv){
    struct cmd* handler;
    handler = builtin_commands;
    printf("Available Commands\n");
    while(handler->name != NULL) {
        printf(" * %s\n",handler->name);
        handler++;
    }
    test_nohandler(0, NULL);
    return 0;
}

int mall_info(int argc, char** argv){
    print_mallinfo();
    return 0;
}

int mem_info(int argc, char** argv){
    sys_meminfo();
    return 0;
}

int print_pid(int argc, char **argv){
    printf("%d\n",getpid());
    return 0;
}

int ps(int argc, char **argv){
    return sys_ps();
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

    printf("Uptime is %dd %dh %dm %d.%02ds\n", days, hours, minutes, seconds, ticks%60);
    printf("user time %d.%d seconds, system time %d.%02d seconds\n",
                            tbuf.tms_utime  / tick_rate , (tbuf.tms_utime) %60,
                            tbuf.tms_stime / tick_rate , (tbuf.tms_stime) %60);
    return 0;
}

/**
 * Handles any unknown command.
 **/
int generic(int argc, char **argv) {
    //Quietly ignore empty file paths
    if(argc == 0)
        return 0;

    printf("Unknown command '%s'\r\n", argv[0]);
    return -1;
}



int exec_cmd(char *line, int tpipe[2]){
    struct cmdLine cmd;
    int ret;
    char *p;
    char* buf;
    struct cmd *handler = NULL;

    ret = parse(line,&cmd);

    if(strcmp(line, "exit") == 0){
        printf("Child %d [parent %d] exits\n",getpid(),getppid());
        exit(0);
    }

    if(cmd.env && cmd.env_val){ //if a new environment variable is set
        buf = malloc(MAX_LINE);

        // printf("set env %s %s\n", cmd.env, cmd.env_val);
        if(parse_quotes(cmd.env_val, buf))
            goto err_free_buf;

        setenv(cmd.env, buf, 1);
        err_free_buf:
            free(buf);
        return 0;
    }

    //Decode command
    handler = builtin_commands;
    while(handler->name != NULL && strcmp(cmd.argv[0], handler->name)) {
        handler++;
    }
    //Run it
    handler->handle(cmd.argc, cmd.argv);
    return 0;
}

void main() {
    int ret;
    char *c;
    char *end_buf;

    c = buf;
    end_buf = c + MAX_LINE -2;
    while(1) {
        printf("WINIX> ");
        c = buf;
        //Read line from terminal
        while( c < end_buf) {
            ret = getchar();     //read
            
            if(ret == EOF){
                if(errno == EINTR){
                    perror("getc() is interrupted");
                    printf("WINIX> ");
                }
                continue;
            }
            
            if(ret == '\r'){//test for end
                break;
            }  
                    

            if ((int)ret == 8) { //backspace
                if(c > buf){
                    putchar(ret);
                    c--;
                }
                continue;
            }
            
            *c++ = ret;
            putchar(ret);         //echo
        }
        
        *c = '\0';
        putchar('\n');
        
        exec_cmd(buf, NULL);     
    }
    exit(EXIT_SUCCESS);
}
