sources = slab.c mem_map.c bitmap.c sys_stdio.c
compile = slab.s mem_map.s bitmap.s sys_stdio.s
assemble = slab.o mem_map.o bitmap.o kwramp.o sys_stdio.o

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
