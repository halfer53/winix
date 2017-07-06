CC = wcc
INCLUDE_DIR = ../include
# INCLUDES = $(wildcard $(INCLUDE_DIR)/*.h $(INCLUDE_DIR)/sys/*.h )

PROGS = 
SOURCES = $(PROGS:=.c)
ASSEMBLY = $(PROGS:=.s)
OBJECT = $(PROGS:=.o)

wramp: $(OBJECT)
$(ASSEMBLY): %.s: %.c
	$(CC) -S -I$(INCLUDE_DIR) -o $@ $<

$(OBJECT): %.o: %.s
	wasm $< -o $@

depend:
	makedepend -I$(INCLUDE_DIR) $(SOURCES)

clean:
	-rm -f $(OBJECT) $(ASSEMBLY)

.DELETE_ON_ERROR:
# DO NOT DELETE
