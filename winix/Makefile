sources = slab.c mem_map.c bitmap.c sys_stdio.c mem_alloc.c signal.c
compile = slab.s mem_map.s bitmap.s sys_stdio.s mem_alloc.s	signal.s
assemble = slab.o mem_map.o bitmap.o kwramp.o sys_stdio.o mem_alloc.o signal.o

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
	-rm $(compile) $(assemble)

.DELETE_ON_ERROR:
