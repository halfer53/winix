sources = 
compile = 
assemble = 

all: $(assemble)

$(compile): %.s: %.c
	wcc -S -I../include -o $@ $<

$(debug): %.s: %.c
	wcc -b -S -I../../include -o $@ $<

$(assemble): %.o: %.s
	wasm $< -o $@

depend:
	makedepend $(sources)

clean:
	-rm -f $(compile) $(assemble)

.DELETE_ON_ERROR:
