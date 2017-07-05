CC = wcc
PROGS = slab mm bitmap sys_stdio wini_ipc kdebug
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o) kwramp.o

wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c
	$(CC) -S -I../include -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

$(link): $(assemble)
	wlink -o $(link) $(assemble)

depend:
	makedepend $(sources)

clean:
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR:
