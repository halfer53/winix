#include <type.h>
#include <stdlib.h>
#include <sys/syscall.h>


void* malloc (size_t size){
	return sbrk(size);
}

void free(void *ptr){

}



