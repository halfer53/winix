sources = 
compile = 
assemble = 

all: $(assemble)

$(compile): %.s: %.c
	wcc -S -I../include -o $@ $<

$(assemble): %.o: %.s
	wasm $< -o $@

depend:
	makedepend $(sources)

clean:
	-rm $(compile) $(assemble)

.DELETE_ON_ERROR:
