CC = wcc
PROGS = main proc system exception idle clock sched
INCLUDE_DIR = ../include
# INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/winix/*.h $(INCLUDE_DIR)/sys/*.h $(INCLUDE_DIR)/kernel/*.h)

SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

wramp:$(OBJECT)
	$(MAKE) -C system
$(ASSEMBLY): %.s: %.c $(INCLUDES)
	$(CC) -S -I$(INCLUDE_DIR) -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	$(MAKE) -C system depend
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	$(MAKE) -C system clean
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR:# DO NOT DELETE


