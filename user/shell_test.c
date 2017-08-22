#include "shell.h"

void stack_overflow(int a){
	stack_overflow(a);
}

int test_so(){
	if(!fork()){
		printf("child recursively calling itself\n");
		stack_overflow(1);
	}else{
		wait(NULL);
	}
	return 0;
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

int test_alarm(int input){
	int i;
	seconds = input;
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

int test_signal(int input){
	int i;
	pid_t pid;
	pid_t fr;
	seconds = input;
	signal(SIGALRM,signal_handler);
	alarm(seconds);
	return 0;
}

ucontext_t mcontext;
#define THREAD_STACK_SIZE	56

void func(int arg) {
  	printf("Hello World! I'm thread %d\n",arg);
}

int test_thread(int num){
	int i,j;
	int count = 1;
	void **thread_stack_op;
	ucontext_t *threads; 
	ucontext_t *cthread;

	//ucontext represents the context for each thread
	threads = malloc(sizeof(ucontext_t) * num);
	if(threads == NULL)
		goto end;
	
	//thread_stack_op saves the original pointer returned by malloc
	//so later we can use it to free the malloced memory
	thread_stack_op = malloc(sizeof(int) * num);
	if(thread_stack_op == NULL)
		goto end_free_threads;
		
	cthread = threads;
	//Allocate stack for each thread
	for( i = 0; i < num; i++){
		if ((thread_stack_op[i] =  malloc(THREAD_STACK_SIZE)) != NULL) {
			cthread->uc_stack.ss_sp = thread_stack_op[i];
			cthread->uc_stack.ss_size = THREAD_STACK_SIZE;
			cthread->uc_link = &mcontext;
			makecontext(cthread,func,1,count++);
			
			if(i%50 == 0)
				putc('!');
		}else{
			goto end_free_all;
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
		swapcontext(&mcontext,cthread++);
	}
	
	end_free_all:
		for( j = 0; j < i; j++){
			free(thread_stack_op[j]);
		}
		free(thread_stack_op);
	end_free_threads:
		free(threads);
	end:
		if(errno == ENOMEM)
			perror("malloc failed");
	return 0;
}


int test_malloc(){
	
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
	print_mallinfo();
	free(p5);
	free(p6);
	free(p2);
	free(p8);
	print_mallinfo();
  
  return 0;
}

int test_fork(){
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
