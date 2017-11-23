REFORMAT = reformat_srec
GEN_BIN = gen_bin_code
	
all:$(REFORMAT).class $(GEN_BIN)
$(REFORMAT).class $(GEN_BIN): $(REFORMAT).java $(GEN_BIN).c
	$(Q)javac $(REFORMAT).java
	$(Q)gcc $(GEN_BIN).c -o $(GEN_BIN)
	$(Q)ln -f -s `pwd`/kdbg.py `pwd`/..

clean:
	$(Q)-rm -f $(REFORMAT).class $(GEN_BIN)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t TOOLS"
endif

	
	
