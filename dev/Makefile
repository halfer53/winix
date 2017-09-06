sources = dev.c
compile = dev.s
assemble = dev.o

all: $(assemble)

$(compile): %.s: %.c
	@wcc -S -N -I../../include -o $@ $<
	@echo "CC \t $<"

$(debug): %.s: %.c
	@wcc -b -S -N -I../../include -o $@ $<
	@echo "CC \t $<"

$(assemble): %.o: %.s
	@wasm $< -o $@
	@echo "AS \t $<"

$(link): $(assemble)
	wlink -o $(link) $(assemble)

depend:
	makedepend $(sources)

clean:
	-rm $(compile) $(assemble)

.DELETE_ON_ERROR:
