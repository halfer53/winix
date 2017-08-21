INCLUDE_DIR = ../../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)


PROGS = sys_alarm sys_brk sys_execve sys_exit sys_fork \
		sys_sigaction sys_sigreturn sys_wait sys_kill sys_time\
		sys_winfo sys_printf sys_getc sys_getpid\
		

SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

MAKEDEPEND = gcc -M $(CPPFLAGS) -o $*.Td $<

wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c $(INCLUDES)
	wcc $(CFLAGS) -S -N -I$(INCLUDE_DIR) -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	-rm -f $(ASSEMBLY) $(OBJECT)

.DELETE_ON_ERROR:# DO NOT DELETE

