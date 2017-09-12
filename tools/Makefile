REFORMAT = reformat_srec
GEN_BIN = gen_bin_code

tools: $(REFORMAT).class $(GEN_BIN) 
$(REFORMAT).class $(GEN_BIN): $(REFORMAT).java $(GEN_BIN).c
	javac $(REFORMAT).java
	gcc $(GEN_BIN).c -o $(GEN_BIN)

clean:
	$(Q)-rm -f $(OBJECT) $(ASSEMBLY)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t TOOLS"
endif

	
	
