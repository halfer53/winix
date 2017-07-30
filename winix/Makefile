INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

PROGS = slab mm bitmap sys_stdio wini_ipc kdebug timer
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o) kwramp.o

wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c $(INCLUDES)
	$(CC) $(CFLAGS) $(C_DEF) -I$(INCLUDE_DIR) -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR:
