/**
 * 
 * Shell testing route
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
#include "shell.h"

CMD_PROTOTYPE(test_malloc);
CMD_PROTOTYPE(test_so);
CMD_PROTOTYPE(test_float);
CMD_PROTOTYPE(test_fork);
CMD_PROTOTYPE(test_thread);
CMD_PROTOTYPE(test_alarm);
CMD_PROTOTYPE(test_signal);
CMD_PROTOTYPE(test_nohandler);
CMD_PROTOTYPE(test_vfork);

struct cmd_internal test_commands[] = {
    { test_malloc, "malloc"}, 
    { test_so, "stack"},
    { test_float, "float"},
    { test_thread, "thread"},
    { test_alarm, "alarm"},
    { test_signal, "signal"},
    { test_vfork, "vfork"},
    { test_nohandler, NULL},
};


int test_nohandler(int argc, char** argv){
    int i;
    struct cmd_internal* handler;
    handler = test_commands;
    printf("Available Test Options\n");
    for( i = 0; i < sizeof(test_commands) / sizeof(struct cmd_internal) - 1; i++){
        if(handler->name)
            printf(" * %s\n",handler->name);
        handler++;
    }
    printf("e.g. \"test alarm 1\", \"test thread 100\" \n");
    return 0;
}

int test_vfork(int argc, char **argv){
    pid_t pid = vfork();
    if(pid == 0){
        _exit(0);
    }
    printf("parent awaken\n");
    return 0;
}

int test_general(int argc, char **argv){
    struct cmd_internal* handler;
    handler = test_commands;
    while(handler->name != NULL && strcmp(argv[1], handler->name)) {
        handler++;
    }
    //Run it
    handler->handle(argc-1, argv+1);
    return 0;
}

int test_float(int argc, char **argv){
    int foo;
    signal(SIGFPE, SIG_IGN);
    foo = 1 / 0;
    return 0;
}

void stack_overflow(int a){
    stack_overflow(a);
}

int test_so(int argc, char **argv){
    if(!fork()){//child
        printf("Generating stack overflow ....\n");
        stack_overflow(1);
    }else{
        int status;
        wait(&status);
    }
    return 0;
}


int seconds = 1;
int cont;

void signal_handler(int signum){
    printf("\n%d seconds elapsed\n",seconds);
}

void alarm_handler(int signum){
    printf("\n%d seconds elapsed\n",seconds);
    cont = 0;
}

/**

    The difference between test_alarm and test_signal is that
    test_signal will return to the main shell loop after issuing the
    alarm syscall, thus it will be blocked until a new character arrives.
    After one second, when SIGALRM is scheduled to be delivered, and when
    shell process is still blocked, the kernel will temporarily schedule 
    the process, and call the signal handler. Upon exiting signal handler,
    the shell process will be blocked again.

    On the other side, test_alarm() simply calls a while loop to wait for
    the signal to be delivered, this serves as a simple test case for signal
    but avoids to test on the edge cases where signal is delivered while 
    a process is blocked by a system call.

**/
int test_alarm(int argc, char **argv){
    int i;
    seconds = (argc > 1) ? atoi(argv[1]) : 1;
    signal(SIGALRM,alarm_handler);
    alarm(seconds);
    cont = 1;
    i = 10000;
    while(cont){
        putchar('!');
        while(i--);
        i = 10000;
    }
        
    return 0;
}

int test_signal(int argc, char **argv){
    int i;
    pid_t pid;
    pid_t fr;
    
    seconds = (argc > 1) ? atoi(argv[1]) : 1;
    signal(SIGALRM,signal_handler);
    alarm(seconds);
    return 0;
}

ucontext_t mcontext;
#define THREAD_STACK_SIZE    56

void func(int arg) {
      printf("Hello World! I'm thread %d\n",arg);
}

int test_thread(int argc, char **argv){
    int i,j,num = 2;
    int count = 1;
    void **thread_stack_op;
    ucontext_t *threads; 
    ucontext_t *cthread;

    if(argc > 1)
        num = atoi(argv[1]);
    //ucontext represents the context for each thread
    threads = malloc(sizeof(ucontext_t) * num);
    if(threads == NULL)
        goto err;
    
    //thread_stack_op saves the original pointer returned by malloc
    //so later we can use it to free the malloced memory
    thread_stack_op = malloc(sizeof(int) * num);
    if(thread_stack_op == NULL)
        goto err_free_threads;
        
    cthread = threads;
    //Allocate stack for each thread
    for( i = 0; i < num; i++){
        if ((thread_stack_op[i] =  malloc(THREAD_STACK_SIZE)) != NULL) {
            cthread->uc_stack.ss_sp = thread_stack_op[i];
            cthread->uc_stack.ss_size = THREAD_STACK_SIZE;
            cthread->uc_link = &mcontext;
            makecontext(cthread,func,1,count++);
            cthread++;

            if(i%50 == 0)
                putchar('!');
        }else{
            goto err_free_all;
        }
    }
    putchar('\n');
    
    cthread = threads;
    //scheduling the threads
    //note that we are using user thread library, 
    //so we have to manually schedule all the threads.
    //Currently the scheduling algorithm is just simple a round robin
    for( i = 0; i < num; i++){
        swapcontext(&mcontext,cthread++);
    }
    
    err_free_all:
        for( j = 0; j < i; j++){
            free(thread_stack_op[j]);
        }
        free(thread_stack_op);
    err_free_threads:
        free(threads);
    err:
        if(errno == ENOMEM)
            perror("malloc failed");
    return 0;
}


int test_malloc(int argc, char **argv){
    
    void *p0 = malloc(512);
    void *p1 = malloc(512);
    void *p2 = malloc(1024);
    void *p3 = malloc(512);
    void *p4 = malloc(1024);
    void *p5 = malloc(2048);
    void *p6 = malloc(512);
    void *p7 = malloc(1024);
    void *p8 = malloc(512);
    void *p9 = malloc(1024);
    free(p5);
    free(p6);
    free(p2);
    free(p8);
    print_heap();
  
    return 0;
}
