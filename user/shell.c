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

#define BUF_LEN			64
#define MAX_COMMANDS	5
#define MAX_ARGS		20

//Prototypes
int ps(int argc, char **argv);
int uptime(int argc, char **argv);
int shutdown(int argc, char **argv);
int exit(int argc, char **argv);
int testmalloc(int argc, char **argv);
int test_signal(int argc, char **argv);
int test_thread(int argc, char **argv);
int test_alarm(int argc, char **argv);
int generic(int argc, char **argv);

//Input buffer & tokeniser
static char buf[BUF_LEN];

//Maps strings to function pointers
struct cmd {
	int (*handle)(int argc, char **argv);
	char *name;
};

struct cmdLine{
	char buf[BUF_LEN];
	int argc;
	char cmdStart[6];
	int numCommands;
	char *argv[BUF_LEN / 2];
	int append;
};

//Command handling
struct cmd commands[] = {
	{ uptime, "uptime" },
	{ shutdown, "shutdown" },
	{ exit, "exit" },
	{ ps, "ps" },
	{ testmalloc, "malloc"},
	{ test_signal, "signal"},
	{ test_thread, "thread"},
	{ test_alarm, "alarm"},
	{ generic, NULL }
};
//TODO: ps/uptime/shutdown should be moved to separate programs.

/**
 * Returns true if c is a printable character.
 **/
int isPrintable(int c) {
	return ('!' <= c && c <= '~');
}

int cont;

void alarm_handler(int signum){
	printf("\nAlarm timer received\n");
	cont = 0;
}

int test_alarm(int argc, char **argv){
	int i = 10000;
	int seconds = 1;

	if(argc > 1)
		seconds = atoi(argv[1]);
		
	signal(SIGALRM,alarm_handler);
	alarm(seconds);
	return 0;
}

void sighandler(int signum){
	
	printf("\nSignal received\nChild exit \n");
	sys_exit(0);
}

int test_signal(int argc, char **argv){
	int i;
	pid_t pid;
	pid_t fr;
	int seconds = 1;
	if(argc > 1)
		seconds = atoi(argv[1]);

	if((fr = fork()) == 0){
		signal(SIGALRM,sighandler);
		alarm(seconds);
		i = 10000;
		while(1){
			while(i--);
			putc('!');
			i = 10000;
		}
	}else{
		printf("parent waiting for child %d\n",fr);
		pid = wait(NULL);
		printf("parent awaken by child %d\n",pid);
	}
	return 0;
}

ucontext_t mcontext;
#define THREAD_STACK_SIZE	56

void func(int arg) {
  printf("Thread %d\n",arg);
  printf("THread %d return to main\n");
  setcontext(&mcontext);
}

int test_thread(int argc, char **argv){
	int num = 2;
	int i;
	int count = 1;
	void **thread_stack_op;
	ucontext_t *threads; 
	ucontext_t *cthread;
	if(argc > 1)
		num = atoi(argv[1]);

	printf("num %d\n",num);
	//ucontext represents the context for each thread
	threads = malloc(sizeof(ucontext_t) * num);
	//thread_stack_op saves the original pointer returned by malloc
	//so later we can use it to free the malloced memory
	thread_stack_op = malloc(sizeof(int) * num);
	cthread = threads;
	printf("num %d\n",num);

	//Allocate stack for each thread
	for( i = 0; i < num; i++){
		if ((thread_stack_op[i] =  malloc(THREAD_STACK_SIZE)) != NULL) {
			cthread->ss_sp = (uint32_t *)thread_stack_op[i] + THREAD_STACK_SIZE -1;
			cthread->ss_size = THREAD_STACK_SIZE;
			cthread->ss_flags = 0;
			makecontext(cthread,func,1,count++);
			putc('!');
		}else{
			printf("malloc failed\n");
		}
		cthread++;
	}
	block_overview();

	printf("context has been built\n");
	
	cthread = threads;
	//scheduling the threads
	//note that we are using user thread, so we have to manually schedule all the threads
	//currently the scheduling algorithm is just simple a round robin
	for( i = 0; i < num; i++){
		swapcontext(&mcontext,cthread);
		cthread++;
	}
	
	//free up the malloced memory
	for( i = 0; i < num; i++){
		free(thread_stack_op[i]);
	}
	free(thread_stack_op);
	free(threads);

	block_overview();
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
	return sys_process_overview();
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

	printf("Uptime is %dd %dh %dm %d.%ds, total ticks: %d\r\n", days, hours, minutes, seconds, ticks%100, ticks);
	return 0;
}

/**
 * Shuts down OS.
 **/
int shutdown(int argc, char **argv) {
	printf("Placeholder for SHUTDOWN\r\n");
	return 0;
}

/**
 * Exits the terminal.
 **/
int exit(int argc, char **argv) {
	int status = 0;
	if(argc > 1)
		status = atoi(argv[1]);
	printf("Bye!\r\n");
	return sys_exit(status);
}

/**
 * Handles any unknown command.
 **/
int generic(int argc, char **argv) {
	//Quietly ignore empty file paths
	if(argc == 0)
		return 0;

		if (strcmp("exec",argv[0]) == 0) {
			printf("please drag the srec file onto this windows\n" );
			return exec();
		}else if (strcmp("fork",argv[0]) == 0) {
			int forkid = 0;
			forkid = fork();
			if (forkid != 0)
			{
				printf("I am parent\n");
			}else{
				printf("I am child\n");
			}
			return 0;
		}
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
	int i,ret;
	char *c;
	struct cmd *handler = NULL;
	struct cmdLine sc;

	while(1) {
		printf("WINIX> ");
		c = buf;
		i = 0;
		//Read line from terminal
		while( i < BUF_LEN - 2) {

			ret = getc(); 	//read
			
			if(ret == -1)
				continue;
			
			if(ret == '\r')  //test for end
				break;	

			if ((int)ret == 8) { //backspace
				if (i > 0) {
					putc(ret);
					c--;i--;
				}
				continue;
			}
			*c++ = ret;
			i++;
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
	sys_exit(0);
}
