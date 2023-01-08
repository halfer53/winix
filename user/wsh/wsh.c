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
 * @create date 2016-0919
 * 
*/

#include "wsh.h"

int exec_cmd(char *line);
static char history_file[] = ".history";
static char PREFIX[] = "WINIX> ";

static pid_t pgid;
static pid_t last_pgid;
static pid_t last_stopped_pgid;
struct termios termios;
bool termios_inited = false;

// Prototypes
CMD_PROTOTYPE(slab);
CMD_PROTOTYPE(cmd_kill);
CMD_PROTOTYPE(print_pid);
CMD_PROTOTYPE(mem_info);
CMD_PROTOTYPE(do_trace_syscall);
CMD_PROTOTYPE(do_untrace_syscall);
CMD_PROTOTYPE(help);
CMD_PROTOTYPE(cmd_exit);
CMD_PROTOTYPE(printenv);
CMD_PROTOTYPE(do_cd);
CMD_PROTOTYPE(do_cls);
CMD_PROTOTYPE(do_fg);
CMD_PROTOTYPE(do_stest);


// Command handling
struct cmd_internal builtin_commands[] = {
    { printenv, "env" },
    { do_trace_syscall, "trace"},
    { do_untrace_syscall, "untrace"},
    { slab, "slab"},
    { cmd_kill, "kill"},
    { print_pid, "pid"},
    { mem_info, "free"},
    { cmd_exit, "exit"},
    { help, "help"},
    { help, "?"},
    { do_cd, "cd"},
    { do_cls, "clear"},
    { do_fg, "fg"},
    { do_stest, "stest"},
    { 0, NULL}
};
void init_pgid(){
    pgid = getpgid(0);
}

void init_shell(){
    int ret = setpgid(0, 0);
    init_pgid();
    ret = tcsetpgrp(STDIN_FILENO, pgid);
    assert(ret == 0);
    tcgetattr(STDIN_FILENO, &termios);
    termios_inited = true;
}

void init_shell_sysenv(){
#ifdef __wramp__
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
#endif
}

void restore_shell_sysenv(){
#ifdef __wramp__
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
#endif
}

int append_line_to_history_file (char *filename, char *line){
    int ret;
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd < 0)
        return -errno;
    ret = write(fd, line, strlen(line));
    close(fd);
    return ret;
}

int main(int argc, char *argv[]) {
    init_pgid();

    if(argc > 2 && strcmp(argv[1], "-c") == 0){
        int i;
        char buf[MAX_LINE];
        buf[0] = '\0';
        for(i = 2; i < argc; i++){
            strlcat(buf, argv[i], MAX_LINE);
            if (i < argc - 1)
                strlcat(buf, " ", MAX_LINE);
        }
        return exec_cmd(buf);
    }

    init_shell();
    init_shell_sysenv();

    while(1) {
        int linelen;
        char *line = readline(PREFIX);

        if (line == NULL)
            break;
        
        linelen = strlen(line);
        if (linelen == 0 || line[0] == '#' || line[0] == '\n')
            continue;
        
        if (*line){
            add_history(line);
            append_line_to_history_file(history_file, line);
        }

        if (line[linelen - 1] == '\n')
            line[linelen - 1] = '\0';

        exec_cmd(line);
        free(line);
    }
    if (termios_inited)
        tcsetattr(STDIN_FILENO, TCSANOW, &termios);
    return 0;
}

int search_path(char* path, int len, char* name){
    int ret;
    struct stat statbuf;
    strlcpy(path, "/bin/", len);
    strlcat(path, name, len);
    ret = stat(path, &statbuf);
    if(ret)
        return ret;
    return statbuf.st_mode & S_IFREG ? 0 : -1;
}

#define PIPE_READ   (0)
#define PIPE_WRITE  (1)
#define BUFFER_LEN  (30)

pid_t run_cmd(struct cmdLine *cmd, int i, int *pipe_ptr, int *prev_pipe_ptr){
    int sout;
    pid_t pid;
    int cmd_start = cmd->cmdStart[i];
    char buffer[BUFFER_LEN];

    if(search_path(buffer, BUFFER_LEN, cmd->argv[cmd_start])){
        fprintf(stderr, "Unknown command '%s'\n", cmd->argv[cmd_start]);
        return -1;
    }

    pid = tfork();
    if (pid == -1){
        perror("fork");
        return -1;
    }

    if(pid == 0){ // child, actual command

        // if redirecting input and first command
        if (i == 0 && cmd->infile) { 
            int sin = open(cmd->infile, O_RDONLY);
            dup2(sin,STDIN_FILENO);
            close(sin);
        }
        
        // if redirecting output and last command
        if(i == cmd->numCommands - 1 && cmd->outfile){ 
            int mode = O_WRONLY | O_CREAT;
            if(cmd->append) //if append
                mode |= O_APPEND;
            else //else replace the original document
                mode |= O_TRUNC;
            sout = open(cmd->outfile, mode, 0664);
            dup2(sout,STDOUT_FILENO);
            close(sout);
        }

        restore_shell_sysenv();
        if (termios_inited)
            tcsetattr(STDIN_FILENO, TCSANOW, &termios);

        if (last_pgid){
            setpgid(0, last_pgid);
        }else{
            setpgid(0, 0);
            last_pgid = getpgid(0);
            tcsetpgrp(STDIN_FILENO, last_pgid);
        }

    
        if(cmd->numCommands > 1){
            if((i+1) < cmd->numCommands ){ // not the last command
                dup2(pipe_ptr[PIPE_WRITE], STDOUT_FILENO);
                close(pipe_ptr[PIPE_WRITE]);
                close(pipe_ptr[PIPE_READ]);
                // printf("cmd %d %s pipe  %d %d\n", i, buffer, pipe_ptr[PIPE_READ], pipe_ptr[PIPE_WRITE]);
            }
            // printf("cmd %d %s com %d, res %d\n", i, buffer,  cmd->numCommands, (i+1) < cmd->numCommands);

            if(i > 0){ // not the first command, read previous pipe
                dup2(prev_pipe_ptr[PIPE_READ], STDIN_FILENO);
                close(prev_pipe_ptr[PIPE_READ]);
                close(prev_pipe_ptr[PIPE_WRITE]);
                // printf("cmd %d %s dup read %d\n", i, buffer, prev_pipe_ptr[PIPE_READ]);
            }
        }
        execv(buffer, &cmd->argv[cmd_start]);
        perror("execv");
        exit(1);
    }

    return pid;
}



/**
 * Handles any unknown command.
 **/
int _exec_cmd(struct cmdLine *cmd) {
    int status;
    int i, ret;
    int pipe_fds[10];
    int *pipe_ptr, *prev_pipe_ptr = NULL;
    pid_t jobid;

    if(cmd->argc == 0)
        return 1;

    last_pgid = 0;

    for(i = 0; i < cmd->numCommands; i++){
        pipe_ptr = &pipe_fds[(i * 2)];
        if (i < cmd->numCommands - 1){ // not the last command, create new pipe
            ret = pipe(pipe_ptr);
            if(ret){
                perror("pipe");
                return -1;
            }
        }
        jobid = run_cmd(cmd, i, pipe_ptr, prev_pipe_ptr);
        cmd->job_pid[i] = jobid;
        
        if (prev_pipe_ptr){
            close(prev_pipe_ptr[PIPE_READ]);
            close(prev_pipe_ptr[PIPE_WRITE]);
        }
        prev_pipe_ptr = pipe_ptr;
    }

    for(i = 0; i < cmd->numCommands; i++){
        if (cmd->job_pid[i] == -1)
            continue;
        ret = waitpid(cmd->job_pid[i], &status, WUNTRACED);
        if(WIFSTOPPED(status)){
            // printf("Stopped pg %d\n", last_pgid);
            last_stopped_pgid = last_pgid;
        }
    }

    tcsetpgrp(STDIN_FILENO, pgid);
    return WEXITSTATUS(status);
}

int exec_cmd(char *line){
    struct cmdLine cmd;
    char* buffer;
    struct cmd_internal *handler = NULL;

    (void)parse(line,&cmd);

    if(cmd.env && cmd.env_val){ // if a new environment variable is set
        buffer = malloc(MAX_LINE);
        parse_quotes(cmd.env_val, buffer, MAX_LINE);
        
        if(*buffer){
            printf("setenv %s=%s\n", cmd.env, buffer);
            setenv(cmd.env, buffer, 1);
        }
        free(buffer);
        return 0;
    }

    // Decode command
    handler = builtin_commands;
    while(handler && handler->name && strcmp(cmd.argv[0], handler->name)) {
        handler++;
    }
    // Run it
    if(handler->name){
        return handler->handle(cmd.argc, cmd.argv);
    }
    return _exec_cmd(&cmd);
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
    char *endptr;
    int pidposition = 1;
    if(argc < 2){
        printf("kill <-signal> [pid]\n");
        return 1;
    }
        
    if(*argv[1] == '-'){
        signum = strtol(argv[1] + 1, &endptr, 10);
        if(*endptr){
            fprintf(stderr, "Invalid number '%s'\n", argv[1] + 1);
            return 1;
        }
        pidposition = 2;
    }

    pid = strtol(argv[pidposition], &endptr, 10);
    if(*endptr){
        fprintf(stderr, "Invalid number '%s'\n", argv[2]);
        return 1;
    }

    if(kill(pid,signum))
        perror("kill ");
    return 0;
}

int do_cd(int argc, char** argv){
    int ret;
    if(argc < 2)
        return 1;
    ret = chdir(argv[1]);
    if(ret)
        perror("");
    return ret;
}

int help(int argc, char** argv){
    struct cmd_internal* handler;
    handler = builtin_commands;
    printf("Available Commands\n");
    while(handler && handler->name != NULL) {
        printf(" * %s\n",handler->name);
        handler++;
    }
    return 0;
}

int do_stest(int argc, char** argv){
    static char test_str[] = "ls -lah /bin | grep snake | wc | cat";
    // static char test_str[] = "test deadlock";

    int i, ret;
    for(i = 0; i < 10; i++){
        if((ret = exec_cmd(test_str))){
            break;
        }
    }
    return 0;
}

// byte stream for \e[0;0H\e[2J
char cls[] = {0x1b, 0x5b, 0x30, 0x3b, 0x30, 0x48, 0x1b, 0x5b, 0x32, 0x4a, 0};

int do_cls(int argc, char** argv){
    printf("%s", cls);
    return 0;
}

int do_trace_syscall(int argc, char** argv){
    printf("Printing syscall tracing in Serial Port 2\n");
    return enable_syscall_tracing();
}

int do_untrace_syscall(int argc, char** argv){
    printf("Syscall tracking disabled\n");
    return disable_syscall_tracing();
}

// Print the system wise memory info
int mem_info(int argc, char** argv){
    (void)wramp_syscall(WINFO, WINFO_MEM);
    return 0;
}

// current pid
int print_pid(int argc, char **argv){
    printf("%d\n",getpid());
    return 0;
}

// list all the processes in the system
int slab(int argc, char **argv){
    return wramp_syscall(WINFO, WINFO_SLAB);
}

int cmd_exit(int argc, char **argv){
    int status = 0;
    char *endptr;
    if(argc > 1){
        status = strtol(argv[1], &endptr, 10);
        if(*endptr){
            fprintf(stderr, "Invalid number '%s'\n", argv[1]);
            return 1;
        }
    }

    printf("Bye!\n");
    // printf("Child %d [parent %d] exits\n",getpid(),getppid());
    exit(status);
    return 1;
}

int do_fg(int argc, char **argv){
    int status;
    int ret;

    tcsetpgrp(STDIN_FILENO, last_stopped_pgid);
    ret =  kill(-last_stopped_pgid, SIGCONT);
    if(ret == 0){
        ret = waitpid(-last_stopped_pgid, &status, WUNTRACED);
    }
    tcsetpgrp(STDIN_FILENO, pgid);
    return ret;
}




