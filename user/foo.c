#include <kernel/kernel.h>

proc_t ptable[5];

int main(int argc, char const *argv[]) {
	int i;
	proc_t *p;
	proc_t *end = &proc_table[0];
	end += 5;


	for(p = &proc_table[0]; p < end; p++){
		p->proc_nr = 1;
	}
	
  return OK;
}
