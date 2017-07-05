CC = wcc
PROGS = sys_alarm sys_brk sys_exec sys_exit sys_fork sys_sbrk \
		sys_sigaction sys_sigreturn sys_wait sys_sigsend sys_getc\
		sys_time sys_ps sys_putc sys_printf sys_getpid
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c
	$(CC) -S -I../../include -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

# $(link): $(assemble)
# 	wlink -o $(link) $(assemble)

depend:
	makedepend $(sources)

clean:
	-rm -f $(ASSEMBLY) $(OBJECT)

.DELETE_ON_ERROR: