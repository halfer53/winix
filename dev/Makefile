sources = dev.c
compile = dev.s
assemble = dev.o

all: $(assemble)

$(compile): %.s: %.c
	wcc -S -I../../include -o $@ $<

$(debug): %.s: %.c
	wcc -b -S -I../../include -o $@ $<

$(assemble): %.o: %.s
	wasm $< -o $@

$(link): $(assemble)
	wlink -o $(link) $(assemble)

depend:
	makedepend $(sources)

clean:
	-rm $(compile) $(assemble)

.DELETE_ON_ERROR:
