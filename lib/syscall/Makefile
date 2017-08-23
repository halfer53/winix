INCLUDE_DIR = ../../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h)

PROGS = syscall ipc _exit alarm brk_sbrk execve \
		getchar kill printf sigaction sys_uptime \
		sys_winfo wait fork_getpid 
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)

OBJECT = $(PROGS:=.o) wramp_syscall.o

wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c $(INCLUDES)
	wcc $(CFLAGS) -S -N -I$(INCLUDE_DIR) -o $@ $<
	
$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR:
