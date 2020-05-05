REFORMAT = reformat_srec
GEN_BIN = gen_bin_code
	
all:$(REFORMAT).class 
$(REFORMAT).class : $(REFORMAT).java 
	$(Q)javac $(REFORMAT).java
	$(Q)ln -f -s `pwd`/kdbg.py `pwd`/..

clean:
	$(Q)-rm -f $(REFORMAT).class 
ifeq ($(KBUILD_VERBOSE),0)
	@echo "RM \t TOOLS"
endif

	
	
