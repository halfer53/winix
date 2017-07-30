INCLUDE_DIR = ../include
# INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h)

PROGS = stdio string syscall ipc util makecontext
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)

OBJECT = $(PROGS:=.o) wramp_syscall.o ucontext.o debug.o

wramp: $(OBJECT)
	$(MAKE) -C stdlib
$(ASSEMBLY): %.s: %.c $(INCLUDES)
	wcc $(CFLAGS) -S -I$(INCLUDE_DIR) -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(MAKE) -C stdlib clean
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR:
