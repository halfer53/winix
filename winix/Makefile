INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

PROGS = slab mm bitmap sys_stdio wini_ipc kdebug timer sigsend util
FOO = *.c
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o) kwramp.o limits/limits_head.o limits/limits_tail.o
wramp: $(OBJECT)

$(ASSEMBLY): %.s: %.c $(INCLUDES)
	@wcc $(CFLAGS) -S -N -I$(INCLUDE_DIR) -o $@ $<
	@echo "CC \t $<"

$(OBJECT): %.o: %.s
	@wasm $< -o $@
	@echo "AS \t $<"

depend:
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	@-rm -f $(OBJECT) $(ASSEMBLY)
	@echo "RM \t WINIX"

.DELETE_ON_ERROR:
