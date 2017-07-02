sources = slab.c mm.c bitmap.c sys_stdio.c
compile = slab.s mm.s bitmap.s sys_stdio.s
assemble = slab.o mm.o bitmap.o kwramp.o sys_stdio.o

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
