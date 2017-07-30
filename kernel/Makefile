
INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

LIMITS = util/limits_head.o util/limits_tail.o
PROGS = main proc system exception idle clock sched
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

wramp:$(OBJECT) $(INCLUDES) $(LIMITS)
	$(MAKE) -C system

$(LIMITS): %.o: %.s
	wasm $< -o $@

$(ASSEMBLY): %.s: %.c $(INCLUDES)
	$(CC) $(CFLAGS) $(C_DEF) -I$(INCLUDE_DIR) -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	$(MAKE) -C system depend
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(MAKE) -C system clean
	-rm -f $(OBJECT) $(ASSEMBLY) $(LIMITS)

.DELETE_ON_ERROR:# DO NOT DELETE


