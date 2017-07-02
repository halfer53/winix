#ifndef _W_EXEC_H_
#define _W_EXEC_H_

proc_t *exec_new_proc(size_t *lines, size_t length, size_t entry, int priority, char *name);
proc_t *exec_replace_existing_proc(proc_t *p,size_t *lines, size_t length, size_t entry, int priority, char *name);
proc_t *exec_proc(proc_t *p,size_t *lines, size_t length, size_t entry, int priority, char *name);
int winix_load_srec_words_length(char *line);
int winix_load_srec_mem_val(char *line,size_t *memValues,int start_index,int memvalLength);
int exec_read_srec(proc_t *p);

#endif
