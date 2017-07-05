CC = wcc
PROGS = 
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c
	$(CC) -S -I../include -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	makedepend $(sources)

clean:
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR:
