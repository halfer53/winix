WINIXO = winix/*.o
sources = main.c proc.c system.c exception.c idle.c sys_memory.c exec.c clock.c signal.c wait_exit.c
compile = main.s proc.s system.s exception.s idle.s sys_memory.s exec.s clock.s signal.s wait_exit.s
assemble = main.o proc.o system.o exception.o idle.o sys_memory.o exec.o clock.o signal.o wait_exit.o util/limits_head.o util/limits_tail.o

all: $(assemble)
$(compile): %.s: %.c
	wcc -S -I../include -o $@ $<

$(debug): %.s: %.c
	wcc -b -S -I../../include -o $@ $<

$(assemble): %.o: %.s
	wasm $< -o $@

$(link): $(assemble)
	wlink -o $(link) $(assemble)

depend:
	makedepend $(sources)

clean:
	-rm -f $(compile) $(assemble)

.DELETE_ON_ERROR: