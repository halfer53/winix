/**
 * 
 *  testing route
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:13:47
 * 
*/
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/debug.h>
#include <ucontext.h>
#include <sys/times.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>
#include <sched.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/wait.h>

#define CMD_PROTOTYPE(name)    int name(int argc, char**argv)

// Maps strings to function pointers
struct cmd_internal {
    int (*handle)(int argc, char **argv);
    char *name;
    bool unittest;
};

CMD_PROTOTYPE(test_so);
CMD_PROTOTYPE(test_float);
CMD_PROTOTYPE(test_sigsegv);
CMD_PROTOTYPE(test_coroutine);
CMD_PROTOTYPE(test_eintr);
CMD_PROTOTYPE(test_nohandler);
CMD_PROTOTYPE(test_signal);
CMD_PROTOTYPE(test_while);
CMD_PROTOTYPE(run_all);
CMD_PROTOTYPE(test_timer);
CMD_PROTOTYPE(test_times);
CMD_PROTOTYPE(test_pipes);

struct cmd_internal test_commands[] = {
    { test_so, "stack", true },
    { test_float, "float", true },
    { test_coroutine, "coroutine", true },
    { test_sigsegv, "null", true },
    { test_eintr, "eintr", true },
    { test_timer, "timer", true },
    { test_times, "times", true },
    { test_signal, "signal", true },
    { test_pipes, "pipe", true },
    { test_while, "while", false },
    { run_all, "run", false },
    { test_nohandler, NULL, false},
    {0}
};

int main(int argc, char **argv){
    struct cmd_internal* handler;
    if(!argv[1])
        return test_nohandler(argc-1, argv+1);

    handler = test_commands;
    while(handler && handler->name != NULL && strcmp(argv[1], handler->name)) {
        handler++;
    }
    // Run it
    return handler->handle(argc-1, argv+1);
}

int test_nohandler(int argc, char** argv){
    struct cmd_internal* handler;
    handler = test_commands;
    printf("Available Test Options\n");
    while(handler->name){
        printf(" * %s\n",handler->name);
        handler++;
    }
    return 0;
}

int run_all(int argc, char** argv){
    struct cmd_internal* handler;
    char *handler_argv[2];
    int ret;
    handler = test_commands;
    handler_argv[1] = NULL;

    while(handler->name){
        if (handler->unittest){
            handler_argv[0] = handler->name;
            ret = handler->handle(1, handler_argv);
            printf("%s return %d\n---\n\n", handler->name, ret);
            assert(ret == 0);
        }
        handler++;
    }

    return 0;
}

int sig_sum;

void usr_handler(int signum){
    sig_sum += signum;
    printf("SIGUR1 start\n");
    raise(SIGUSR2);
    printf("SIGUR1 end\n");
}

void usr2_handler(int signum){
    sig_sum += signum;
    printf("SIGUR2 start\n");
    raise(SIGINT);
    printf("SIGUR2 end\n");
}

void int_handler(int signum){
    sig_sum += signum;
    printf("SIGINT start\n");
    raise(SIGTERM);
    printf("SIGINT end\n");
}

void term_handler(int signum){
    sig_sum += signum;
    printf("SIGTERM start\n");
    printf("SIGTERM end\n");
}

int test_signal(int argc, char **argv){
    struct sigaction sa;
    sigset_t set, prevset;
    sig_sum = 0;

    // Check out what would happen after changing
    // the following sigemptyset to sigfillset
    // explain why

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    // setup signal handlers
    sa.sa_handler = usr_handler;
    sigaction(SIGUSR1, &sa, NULL);
    sa.sa_handler = usr2_handler;
    sigaction(SIGUSR2, &sa, NULL);
    sa.sa_handler = int_handler;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = term_handler;
    sigaction(SIGTERM, &sa, NULL);
    
    sigfillset(&set);
    sigprocmask(SIG_SETMASK, &set, &prevset);

    // send SIGUSR1 to itself
    // since SIGUSR1 is currently blocked by sigprocmask
    // SIGUSR1 will be pended until sigsuspend
    raise(SIGUSR1);
    assert(sig_sum == 0);

    // check pending signals
    sigpending(&set);
    assert(sigismember(&set, SIGUSR1));
    assert(sig_sum == 0);

    // unblock all pending signals
    printf("signal handler usr1 should be called after this\n");
    sigsuspend(&prevset);
    assert(sig_sum == SIGUSR1 + SIGUSR2 + SIGTERM + SIGINT);
    return 0;
}

int test_float(int argc, char **argv){
    int foo = 0, zero = 0;
    signal(SIGFPE, SIG_IGN);
    
    foo = 1 / zero;
    printf("dividing foo %d by 0\n", foo);

    signal(SIGFPE, SIG_DFL);
    return 0;
}

void stack_overflow(int a) {
    stack_overflow(a);
}

pid_t do_fork(){
    return fork();
}

pid_t do_tfork(){
    return tfork();
}

#define FORK_LEN    2
int test_so(int argc, char **argv){
    pid_t (*fork_function_array[FORK_LEN])() = {do_fork, do_tfork};
    int i = 0;

    for(i = 0; i < FORK_LEN; i++){
        pid_t pid = fork_function_array[i]();
        if(!pid){// child
            wramp_syscall(WINFO, WINFO_NO_GPF);
            printf("Generating stack overflow for %d ....\n", i);
            stack_overflow(1);
        }else{
            int status;
            wait(&status);
            assert(WIFSIGNALED(status));
            assert(WTERMSIG(status) == SIGSEGV);
        }
    }
    
    return 0;
}

void sigsegv_handler(int signum){
    printf("sigsegv handler invoked %d\n", signum);
}

int test_sigsegv(int argc, char **argv){
    char *p = (char *)NULL + 1;
    int pid;
    int result;
    if ((pid = tfork()) == 0){
        signal(SIGSEGV, sigsegv_handler);
        printf("%s", p);
        signal(SIGSEGV, SIG_DFL);
        exit(0);
    }else{
        wait(&result);
        // printf("%d %d %d\n", result, WEXITSTATUS(result), WIFSIGNALED(result));
        assert(WIFEXITED(result));
        assert(WEXITSTATUS(result) == 0);
    }
    return 0;
}

bool alarm_handler_called = false;

void alarm_handler(int signum){
    printf("alarm handler triggered\n");
    alarm_handler_called = true;
}

int test_eintr(int argc, char **argv){
    int status;
    char *endptr;
    if(!tfork()){
        char __buffer[10];
        int ret;
        clock_t seconds = 0;
        suseconds_t microseconds = 1000;
        struct itimerval itv;


        memset(&itv, 0, sizeof(itv));
        alarm_handler_called = false;

        if (argc > 1){
            seconds = strtol(argv[1], &endptr, 10);
            if (*endptr){
                fprintf(stderr, "Invalid number: %s\n", argv[1]);
                return 1;
            }
        }
        
        itv.it_value.tv_sec = seconds;
        itv.it_value.tv_usec = microseconds;

        signal(SIGALRM, alarm_handler);
        setitimer(ITIMER_REAL, &itv, NULL);

        ret = read(STDIN_FILENO, __buffer, 10 * sizeof(char));
        assert(ret == -1);
        assert(errno == EINTR);
        assert(alarm_handler_called == true);
        exit(0);
    }else{
        wait(&status);
        assert(WIFEXITED(status));
        assert(WEXITSTATUS(status) == 0);
    }
    return 0;
}

int test_timer(int argc, char ** argv){
    suseconds_t microseconds = 5000;
    struct itimerval itv;
    struct sigaction act;
    memset(&itv, 0, sizeof(itv)); 
    memset(&act, 0, sizeof(act));
    alarm_handler_called = false;

    if(!tfork()){
        itv.it_value.tv_usec = microseconds;
        itv.it_interval = itv.it_value;

        act.sa_handler = alarm_handler;
        act.sa_flags = SA_RESETHAND;

        sigaction(SIGALRM, &act, NULL);
        setitimer(ITIMER_REAL, &itv, NULL);
        while(1);
    }else{
        int status;
        wait(&status);
        assert(alarm_handler_called == true);
        assert(WIFSIGNALED(status));
        assert(WTERMSIG(status) == SIGALRM);

    }
    return 0;
}

ucontext_t mcontext;
#define COROUTINE_STACK_SIZE    56
int sum;

void func(int arg) {
      printf("Hello World! I'm coroutine %d\n",arg);
      sum += arg;
}

int test_coroutine(int argc, char **argv){
    int i,j,num = 2;
    int count = 1;
    int cumulative = 0;
    void **thread_stack_op;
    ucontext_t *coroutines_list; 
    ucontext_t *coroutine;
    char *endptr;
    sum = 0;

    if(argc > 1){
        num = strtol(argv[1], &endptr, 10);
        if (*endptr){
            fprintf(stderr, "Invalid number: %s\n", argv[1]);
            return 1;
        }
    }
        
    // ucontext represents the context for each thread
    coroutines_list = malloc(sizeof(ucontext_t) * num);
    if(coroutines_list == NULL)
        goto err;
    
    // thread_stack_op saves the original pointer returned by malloc
    // so later we can use it to free the malloced memory
    thread_stack_op = malloc(sizeof(int) * num);
    if(thread_stack_op == NULL)
        goto err_free_coroutines;
        
    coroutine = coroutines_list;
    // Allocate stack for each thread
    for( i = 0; i < num; i++){
        if ((thread_stack_op[i] =  malloc(COROUTINE_STACK_SIZE)) != NULL) {
            coroutine->uc_stack.ss_sp = thread_stack_op[i];
            coroutine->uc_stack.ss_size = COROUTINE_STACK_SIZE;
            coroutine->uc_link = &mcontext;
            cumulative += count;
            makecontext(coroutine,func,1,count++);
            coroutine++;

            if(i && i % 50 == 0)
                putchar('!');
        }else{
            goto err_free_all;
        }
    }
    putchar('\n');
    
    coroutine = coroutines_list;
    // scheduling the coroutines
    // note that we are using user thread library, 
    // so we have to manually schedule all the coroutines.
    // Currently the scheduling algorithm is just simple a round robin
    for( i = 0; i < num; i++){
        swapcontext(&mcontext,coroutine++);
    }
    assert(sum == cumulative);
    
    err_free_all:
        for( j = 0; j < i; j++){
            free(thread_stack_op[j]);
        }
        free(thread_stack_op);
    err_free_coroutines:
        free(coroutines_list);
    err:
        if(errno == ENOMEM)
            perror("malloc");
    return 0;
}

int test_while(int argc, char **argv){
    struct timespec ts;
    memset(&ts, 0, sizeof(struct timespec));
    ts.tv_sec = 1;
    while(1){
        printf("a");
        nanosleep(&ts, NULL);
    }
    return 0;
}

int test_times(int argc, char **argv){
    struct tms tms;
    clock_t start, end;
    struct timespec req;
    
    start = times(&tms);
    assert(start > 0);
    assert(tms.tms_utime > 0);
    assert(tms.tms_stime > 0);
    end = times((struct tms *)1);
    assert(end == -1 && errno == EFAULT);
    
    memset(&req, 0, sizeof(struct timespec));
    req.tv_nsec = 100000000;
    nanosleep(&req, NULL);
    end = times(NULL);
    assert(end > 0 && end > start);
    return 0;
}

int test_pipes(int argc, char *argv[]){
    int status, ret;
    int pipefd[2];
    char buffer[100];
    char *wsh_argv[] = {"/bin/wsh", "-c", "echo abc\\\\ndef\\\\nghn | tail -n 2 | grep def", NULL};
    assert(pipe(pipefd) == 0);

    if (!tfork()){
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execv("/bin/wsh", wsh_argv);
        exit(1);
    }
    wait(&status);
    assert(WIFEXITED(status));
    assert(WEXITSTATUS(status) == 0);

    memset(buffer, 0, 100);
    ret = read(pipefd[0], buffer, 100);
    assert(ret == 4);
    assert(strcmp(buffer, "def\n") == 0);
    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}


