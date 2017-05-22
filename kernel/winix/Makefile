sources = slab.c mem_map.c bitmap.c
compile = slab.s mem_map.s bitmap.s
assemble = slab.o mem_map.o bitmap.o

all: $(assemble)

$(compile): %.s: %.c
	wcc -S -I../../include -o $@ $<

$(assemble): %.o: %.s
	wasm $< -o $@

$(link): $(assemble)
	wlink -o $(link) $(assemble)

depend:
	makedepend $(sources)

clean:
	-rm $(compile) $(assemble)

.DELETE_ON_ERROR:
