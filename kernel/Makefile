WINIXO = winix/*.o
sources = main.c proc.c system.c exception.c idle.c sys_memory.c sys_stdio.c exec.c bitmap.c message_queue.c
compile = main.s proc.s system.s exception.s idle.s sys_memory.s sys_stdio.s exec.s bitmap.s message_queue.s
assemble = main.o proc.o system.o exception.o idle.o wramp.o sys_memory.o sys_stdio.o exec.o bitmap.o message_queue.o util/limits_head.o util/limits_tail.o

all: $(assemble)
	$(MAKE) -C winix
$(compile): %.s: %.c
	wcc -S -I../include -o $@ $<

$(assemble): %.o: %.s
	wasm $< -o $@

$(link): $(assemble)
	wlink -o $(link) $(assemble)

depend:
	makedepend $(sources)

clean:
	-rm $(compile) $(assemble)

.DELETE_ON_ERROR: