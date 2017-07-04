sources = slab.c mm.c bitmap.c sys_stdio.c wini_ipc.c kdebug.c
compile = slab.s mm.s bitmap.s sys_stdio.s wini_ipc.s kdebug.s
assemble = slab.o mm.o bitmap.o kwramp.o sys_stdio.o wini_ipc.o kdebug.o

all: $(assemble)

$(compile): %.s: %.c
	wcc -S -I../include -o $@ $<

$(assemble): %.o: %.s
	wasm $< -o $@

$(link): $(assemble)
	wlink -o $(link) $(assemble)

depend:
	makedepend $(sources)

clean:
	-rm -f $(compile) $(assemble)

.DELETE_ON_ERROR:
