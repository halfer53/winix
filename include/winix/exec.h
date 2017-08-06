#ifndef _W_EXEC_H_
#define _W_EXEC_H_ 1

struct proc *exec_new_proc(size_t *lines, size_t length, size_t entry, int priority, char *name);
struct proc *exec_replace_existing_proc(struct proc *p,size_t *lines, size_t length, size_t entry, int priority, char *name);
struct proc *exec_proc(struct proc *p,size_t *lines, size_t length, size_t entry, int priority, char *name);
int winix_load_srec_words_length(char *line);
int winix_load_srec_mem_val(char *line,size_t *memValues,int start_index,int memvalLength);
int exec_read_srec(struct proc *p);

#endif
