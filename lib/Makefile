# sources = stdio.c string.c syscall.c ipc.c util.c stdlib.c makecontext.c
# compile = stdio.s string.s syscall.s ipc.s util.s stdlib.s makecontext.s
# assemble = stdio.o string.o syscall.o wramp_syscall.o ipc.o util.o stdlib.o ucontext.o makecontext.o debug.o

CC = wcc
PROGS = stdio string syscall ipc util stdlib makecontext
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o) wramp_syscall.o ucontext.o debug.o

wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c
	$(CC) -S -I../include -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	makedepend $(sources)

clean:
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR:
