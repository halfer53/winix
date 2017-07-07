CC = wcc
INCLUDE_DIR = ../include
# INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h)

PROGS = stdio string syscall ipc util stdlib makecontext atoi errno
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o) wramp_syscall.o ucontext.o debug.o __errno.o

wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c $(INCLUDES)
	$(CC) -S -I$(INCLUDE_DIR) -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR:
