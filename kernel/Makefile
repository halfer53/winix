WINIXO = winix/*.o
sources = main.c proc.c system.c exception.c idle.c sys_memory.c clock.c signal.c sched.c
compile = main.s proc.s system.s exception.s idle.s sys_memory.s clock.s signal.s sched.s
assemble = main.o proc.o system.o exception.o idle.o sys_memory.o clock.o signal.o sched.o util/limits_head.o util/limits_tail.o

all:
	$(MAKE) -C system
	$(MAKE)	compile

compile: $(assemble)
$(compile): %.s: %.c
	wcc -S -I../include -o $@ $<

$(assemble): %.o: %.s
	wasm $< -o $@

$(link): $(assemble)
	wlink -o $(link) $(assemble)

depend:
	makedepend $(sources)

clean:
	$(MAKE) -C system clean
	-rm -f $(compile) $(assemble)

.DELETE_ON_ERROR: