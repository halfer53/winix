CC = wcc
PROGS = main proc system exception idle clock sched sys_memory
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

all:
	$(MAKE) -C system
	$(MAKE)	wramp

wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c
	$(CC) -S -I../include -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	makedepend $(sources)

clean:
	$(MAKE) -C system clean
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR: