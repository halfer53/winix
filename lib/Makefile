sources = stdio.c string.c syscall.c ipc.c util.c stdlib.c makecontext.c
compile = stdio.s string.s syscall.s ipc.s util.s stdlib.s makecontext.s
assemble = stdio.o string.o syscall.o wramp_syscall.o ipc.o util.o stdlib.o ucontext.o makecontext.o

all: $(assemble)

$(compile): %.s: %.c
	wcc -S -I../include -o $@ $<

$(assemble): %.o: %.s
	wasm $< -o $@

depend:
	makedepend $(sources)

clean:
	-rm $(compile) $(assemble)

.DELETE_ON_ERROR:
