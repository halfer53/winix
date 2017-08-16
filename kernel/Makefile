
INCLUDE_DIR = ../include
INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

PROGS = main proc system exception idle clock sched
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

wramp:$(OBJECT) $(INCLUDES) $(LIMITS)
	$(MAKE) -C system

$(ASSEMBLY): %.s: %.c $(INCLUDES)
	wcc $(CFLAGS) -S -N -I$(INCLUDE_DIR) -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	$(MAKE) -C system depend
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(MAKE) -C system clean
	-rm -f $(OBJECT) $(ASSEMBLY) $(LIMITS)

.DELETE_ON_ERROR:# DO NOT DELETE


