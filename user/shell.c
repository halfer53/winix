/**
 * Simple shell for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include "shell.h"

//Input buffer & tokeniser
static char buf[BUF_LEN];

//Prototypes
CMD_PROTOTYPE(ps);
CMD_PROTOTYPE(uptime);
CMD_PROTOTYPE(shell_exit);
CMD_PROTOTYPE(cmd_kill);
CMD_PROTOTYPE(test_general);
CMD_PROTOTYPE(print_pid);
CMD_PROTOTYPE(mem_info);
CMD_PROTOTYPE(mall_info);
CMD_PROTOTYPE(generic);

//Command handling
struct cmd builtin_commands[] = {
	{ uptime, "uptime" },
	{ shell_exit, "exit" },
	{ ps, "ps" },
	{ cmd_kill, "kill"},
	{ test_general, "test"},
	{ print_pid, "pid"},
	{ mem_info, "free"},
	{ mall_info, "mallinfo"},
	{ generic, NULL }
};


int test_general(int argc, char **argv){

	if(strcmp("malloc", argv[1]) == 0)
		return test_malloc();

	if(strcmp("stack_ov", argv[1]) == 0)
		return test_so();
	
	if(strcmp("fork", argv[1]) == 0)
		return test_fork();

	if(strcmp("thread", argv[1]) == 0){
		int num = argc > 2 ? atoi(argv[2]) : 2;
		return test_thread(num);
	}

	if(strcmp("alarm", argv[1]) == 0){
		int num = argc > 2 ? atoi(argv[2]) : 1;
		return test_alarm(num);
	}

	if(strcmp("signal", argv[1]) == 0){
		int num = argc > 2 ? atoi(argv[2]) : 1;
		return test_signal(num);
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
					perror("getc() is interrupted");
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
		handler = builtin_commands;
		while(handler->name != NULL && strcmp(sc.argv[0], handler->name)) {
			handler++;
		}

		//Run it
		handler->handle(sc.argc, sc.argv);
	}
	exit(EXIT_SUCCESS);
}
