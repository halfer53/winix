/**
 * Simple shell for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ucontext.h>

#define CMD_PROTOTYPE(name)	int name(int argc, char**argv)

#define BUF_LEN			32
#define MAX_COMMANDS	5
#define MAX_ARGS		10

//Input buffer & tokeniser
static char buf[BUF_LEN];

struct cmdLine{
	char buf[BUF_LEN];
	int argc;
	char cmdStart[MAX_COMMANDS];
	int numCommands;
	char *argv[MAX_ARGS];
	int append;
};

//Maps strings to function pointers
struct cmd {
	int (*handle)(int argc, char **argv);
	char *name;
};


//Prototypes
CMD_PROTOTYPE(ps);
CMD_PROTOTYPE(uptime);
CMD_PROTOTYPE(shell_exit);
CMD_PROTOTYPE(cmd_kill);
CMD_PROTOTYPE(testmalloc);
CMD_PROTOTYPE(test_signal);
CMD_PROTOTYPE(test_thread);
CMD_PROTOTYPE(test_alarm);
CMD_PROTOTYPE(test_fork);
CMD_PROTOTYPE(test_so);
CMD_PROTOTYPE(test_general);
CMD_PROTOTYPE(print_pid);
CMD_PROTOTYPE(mem_info);
CMD_PROTOTYPE(mall_info);
CMD_PROTOTYPE(generic);

//Command handling
struct cmd commands[] = {
	{ uptime, "uptime" },
	{ shell_exit, "exit" },
	{ ps, "ps" },
	{ cmd_kill, "kill"},
	{ testmalloc, "malloc"},
	{ test_thread, "thread"},
	{ test_alarm, "alarm"},
	{ test_signal, "signal"},
	{ test_fork, "fork"},
	{ test_so, "stackov"},
	{ test_general, "test"},
	{ print_pid, "pid"},
	{ mem_info, "meminfo"},
	{ mall_info, "mallinfo"},
	{ generic, NULL }
};

/**
 * Returns true if c is a printable character.
 **/
int isPrintable(int c) {
	return ('!' <= c && c <= '~');
}

int mall_info(int argc, char** argv){
	block_overview();
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

#define TEST_LEN	35

int test_general(int argc, char **argv){
	int* hb[TEST_LEN];
	
	int i;
	

	for(i=0; i< TEST_LEN; i++){
		hb[i] = malloc(56);
	}
	// for(i = 0 ; i<TEST_LEN; i++){
	// 	free(hb[i]);
	// }
	// block_overview();
	return 0;
}

void stack_overflow(int a){
	stack_overflow(a);
}

int test_so(int argc, char **argv){
	if(!fork()){
		printf("child recursively calling itself\n");
		stack_overflow(1);
	}else{
		wait(NULL);
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

int seconds;
int cont;

void signal_handler(int signum){
	printf("\n%d seconds elapsed\n",seconds);
}

void alarm_handler(int signum){
	printf("\n%d seconds elapsed\n",seconds);
	cont = 0;
}

int test_alarm(int argc, char **argv){
	int i;
	seconds = 1;
	if(argc > 1)
		seconds = atoi(argv[1]);

	signal(SIGALRM,alarm_handler);
	alarm(seconds);
	cont = 1;
	i = 10000;
	while(cont){
		putc('!');
		while(i--);
		i = 10000;
	}
		
	return 0;
}

int test_signal(int argc, char **argv){
	int i;
	pid_t pid;
	pid_t fr;
	seconds = 1;
	if(argc > 1)
		seconds = atoi(argv[1]);
	signal(SIGALRM,signal_handler);
	alarm(seconds);
	return 0;
}

ucontext_t mcontext;
#define THREAD_STACK_SIZE	56

void func(int arg) {
  	printf("Hello World! I'm thread %d\n",arg);
  	setcontext(&mcontext);
}

int test_thread(int argc, char **argv){
	int num = 2;
	int i,j;
	int count = 1;
	void **thread_stack_op;
	ucontext_t *threads; 
	ucontext_t *cthread;
	if(argc > 1)
		num = atoi(argv[1]);

	//ucontext represents the context for each thread
	threads = malloc(sizeof(ucontext_t) * num);
	if(threads == NULL){
		perror("malloc failed");
		return -1;
	}
		
	//thread_stack_op saves the original pointer returned by malloc
	//so later we can use it to free the malloced memory
	thread_stack_op = malloc(sizeof(int) * num);
	if(thread_stack_op == NULL){
		free(threads);
		perror("malloc failed");
		return -1;
	}
		
	cthread = threads;

	//Allocate stack for each thread
	for( i = 0; i < num; i++){
		if ((thread_stack_op[i] =  malloc(THREAD_STACK_SIZE)) != NULL) {
			cthread->ss_sp = (uint32_t *)thread_stack_op[i] + THREAD_STACK_SIZE -1;
			cthread->ss_size = THREAD_STACK_SIZE;
			cthread->ss_flags = 0;
			makecontext(cthread,func,1,count++);
			
			if(i%50 == 0)
				putc('!');
		}else{
			perror("malloc failed\n");
			goto end;
		}
		cthread++;
	}
	putc('\n');
	cthread = threads;
	//scheduling the threads
	//note that we are using user thread library, 
	//so we have to manually schedule all the threads.
	//Currently the scheduling algorithm is just simple a round robin
	for( i = 0; i < num; i++){
		swapcontext(&mcontext,cthread);
		cthread++;
	}
	
	end:
	//free up the malloced memory
	free(threads);
	for( j = 0; j < i; j++){
		free(thread_stack_op[j]);
	}
	free(thread_stack_op);
	// block_overview();
	return 0;
}


int testmalloc(int argc, char **argv){
	
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
	block_overview();
	free(p5);
	free(p6);
	free(p2);
	free(p8);
	block_overview();
  
  return 0;
}

int ps(int argc, char **argv){
	return sys_ps();
}

/**
 * Prints the system uptime
 **/
int uptime(int argc, char **argv) {
	int ticks, days, hours, minutes, seconds;

	ticks = sys_uptime();
	seconds = ticks / 60; //TODO: define tick rate somewhere
	minutes = seconds / 60;
	hours = minutes / 60;
	days = hours / 24;

	seconds %= 60;
	minutes %= 60;
	hours %= 24;
	// ticks %= 100;

	printf("Uptime is %dd %dh %dm %d.%ds\n", days, hours, minutes, seconds, ticks%100);
	return 0;
}

/**
 * Exits the terminal.
 **/
int shell_exit(int argc, char **argv) {
	int status = 0;
	if(argc > 1)
		status = atoi(argv[1]);
	printf("Bye!\n");
	printf("Child %d [parent %d] exits\n",getpid(),getppid());
	return exit(status);
}


int test_fork(int argc, char **argv){
	pid_t cpid;
	pid_t ppid;
	ppid = getpid();
	if(cpid = fork()){
		if(cpid == -1){
			printf("fork failed\n");
			return -1;
		}
		printf("parent %d waiting for child %d\n",ppid,cpid);
		cpid = wait(NULL);
		printf("parent %d awakened by child %d\n",ppid,cpid);
	}else{
		printf("Child %d [parent %d] start:\n",getpid(),getppid());
	}
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

#define isspace(c)	(c==' ')

int isseparator(char c){
	return isspace(c); 
}

int parse(char *line, struct cmdLine *sc){
	int argc = 0;
	char *in;
	struct cmd *handler;
	sc->argv[0] = '\0';
	strcpy(sc->buf,line); 
	in = sc->buf;
	sc->cmdStart[0] = 0;    /* the 0th command starts at argv[0] */
	sc->numCommands = 0;

	while(argc < MAX_ARGS-1 && sc->numCommands < MAX_COMMANDS){
		
		while(isspace(*in)) /* skip initial whitespace */
            ++in;
        if(*in == '\0')   /* end-of-line, we're done */
            break;

		switch(*in){
			//place holder for special characters e.g. redirections: >, >>, <
			//case '>'

			default:    /* it's just a regular argument word */
				sc->argv[argc++] = in;
				break;
		}
		
		while(*in && !isseparator(*in))
			in++;
		if(*in == '\0') //if its the end of the input string, then we are done
			break;
		else
			*in++ = '\0'; //terminate the current argument and go to the next one
	}
	sc->argv[argc] = NULL;  // terminate the last command 
    sc->argv[argc + 1] = NULL;  // marker meaning "no more commands" 
	sc->argc = argc;

	if(argc > 0)
        ++sc->numCommands;
	
	if(argc == 0)
		sc->argv[0][0] = '\0';

	return 0;
}

void main() {
	int ret;
	char *c;
	char *end_buf;
	struct cmd *handler = NULL;
	struct cmdLine sc;

	c = buf;
	end_buf = c + BUF_LEN -2;
	while(1) {
		printf("WINIX> ");
		c = buf;
		//Read line from terminal
		while( c < end_buf) {

			ret = getc(); 	//read
			
			if(ret == -1){
				if(errno == EINTR){
					perror("getc() interrupted");
					printf("WINIX> ");
				}
				continue;
			}
			
			if(ret == '\r')  //test for end
				break;	

			if ((int)ret == 8) { //backspace
				if (c > buf) {
					putc(ret);
					c--;
				}
				continue;
			}
			*c++ = ret;
			putc(ret); 		//echo
		}
		*c = '\0';
		putc('\n');
		ret = parse(buf,&sc);

		
		//Decode command
		handler = commands;
		while(handler->name != NULL && strcmp(sc.argv[0], handler->name)) {
			handler++;
		}

		//Run it
		handler->handle(sc.argc, sc.argv);
	}
	exit(EXIT_SUCCESS);
}
