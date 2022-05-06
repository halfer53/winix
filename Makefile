.PHONY := kbuild all clean stat include_build unittest buildlib test

srctree := $(shell pwd)
include tools/Kbuild.include

# Check if V options is set by user, if V=1, debug mode is set
# e.g. make V=1 produces all the commands being executed through
# the building process
ifeq ("$(origin V)", "command line")
	KBUILD_VERBOSE = $(V)
endif
ifndef KBUILD_VERBOSE
	KBUILD_VERBOSE = 0
endif
ifeq ($(KBUILD_VERBOSE),1)
	Q =
else
	MAKEFLAGS += --no-print-directory -s
	Q = @
endif

export NO_MAKE_DEPEND := $(N)
export KBUILD_VERBOSE
export Q
export srctree \\
export SREC_INCLUDE := include_winix/srec
export SREC = $(shell find $(SREC_INCLUDE) -name "*.srec")
export TEXT_OFFSET := 1024
export CURR_UNIX_TIME := $(shell date +%s)

export WINIX_INCLUDE_PATH := -Iinclude_winix
export INCLUDE_PATH := -Iinclude
export WRAMP := -D__wramp__
export COMMON_CFLAGS := -DTEXT_OFFSET=$(TEXT_OFFSET) -D_DEBUG 
export GCC_FLAG := -g -Wall -Werror -pedantic -Wno-discarded-qualifiers -Wno-comment 
export CFLAGS := $(COMMON_CFLAGS) $(WINIX_INCLUDE_PATH) $(GCC_FLAG)

# List of user libraries used by the kernel
KLIB_O = lib/syscall/wramp_syscall.o lib/ipc/ipc.o lib/posix/libgen.o\
		lib/gen/ucontext.o lib/stdlib/atoi.o lib/ansi/index.o \
		lib/syscall/debug.o lib/posix/_sigset.o lib/ansi/rand.o lib/ansi/strl.o\
		lib/ansi/memcpy.o lib/ansi/strcmp.o lib/ansi/strlen.o lib/ansi/memset.o \
		
L_HEAD = winix/limits/limits_head.o
L_TAIL = winix/limits/limits_tail.o
KERNEL_O = winix/*.o kernel/system/*.o kernel/*.o fs/*.o fs/system/*.o driver/*.o include_winix/*.o
ALLDIR = init user kernel fs driver winix
ALLDIR_CLEAN = winix lib init user kernel fs driver include_winix
FS_DEPEND = fs/*.c fs/system/*.c fs/mock/*.c winix/bitmap.c
UNIT_TEST_DEPEND = $(shell find tests -name "*.c" -not -name "utest_runner.c")

DISK = include_winix/disk.c
UTEST_RUNNER = tests/utest_runner.c
START_TIME_FILE = include_winix/startup_time.c
UNIT_TEST = unittest
FSUTIL = fsutil

all:
	$(Q)$(MAKE) buildlib
	$(Q)$(MAKE) kbuild
	$(Q)$(MAKE) include_build
	$(Q)wlink $(LDFLAGS) -Ttext $(TEXT_OFFSET) -v -o winix.srec \
		$(L_HEAD) $(KERNEL_O) $(KLIB_O) $(L_TAIL) > $(SREC_INCLUDE)/winix.verbose
ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t winix.srec"
endif

$(FSUTIL): $(FS_DEPEND) fs/fsutil/*.c lib/ansi/strl.c
ifeq ($(KBUILD_VERBOSE),0)
	@echo "CC \t $(FSUTIL)"
endif
	$(Q)gcc -DFSUTIL $(CFLAGS)  $^ -o $(FSUTIL)

buildlib:
	$(Q)$(MAKE) $(build)=lib

kbuild: $(ALLDIR) $(FSUTIL)
$(ALLDIR): FORCE
	$(Q)$(MAKE) $(build)=$@

$(DISK): $(SREC) $(FSUTIL)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t $(DISK)"
endif
	$(Q)./fsutil -t $(TEXT_OFFSET) -o $(DISK) -s $(SREC_INCLUDE) -u $(CURR_UNIX_TIME)
	
include_build: $(DISK)
	$(Q)echo "unsigned int start_unix_time=$(CURR_UNIX_TIME);\n" > $(START_TIME_FILE)
	$(Q)$(MAKE) $(build)=include_winix


$(UTEST_RUNNER): $(UNIT_TEST_DEPEND) tools/utest_generator.py
	$(Q)python3 tools/utest_generator.py $(UNIT_TEST_DEPEND) > $(UTEST_RUNNER)

$(UNIT_TEST): $(FS_DEPEND) $(UNIT_TEST_DEPEND) $(UTEST_RUNNER) user/bash/parse.c lib/ansi/strl.c
ifeq ($(KBUILD_VERBOSE),0)
	@echo "CC \t $(UNIT_TEST)"
endif
	$(Q)gcc -DFSUTIL $(CFLAGS) $^ -o $(UNIT_TEST)

test: $(UNIT_TEST)
	$(Q)./$(UNIT_TEST)

wsh: user/bash/*.c lib/ansi/strl.c
	$(Q)gcc -DFSUTIL $(COMMON_CFLAGS) $(GCC_FLAG) $^ -o wsh
	
clean:
	$(Q)rm -f $(FSUTIL)
	$(Q)rm -f $(UNIT_TEST)
	$(Q)rm -f $(UTEST_RUNNER)
	$(Q)rm -f $(START_TIME_FILE)
	$(Q)rm -f $(DISK)
	$(Q)$(MAKE) $(cleanall)='$(ALLDIR_CLEAN)'

stat:
	@echo "C Lines: "
	@find . -type d -name "include_winix" -prune -o -name "*.c"  -exec cat {} \; | wc -l
	@echo "Header LoC: "
	@find . -name "*.h" -exec cat {} \; | wc -l
	@echo "Assembly LoC: "
	@find . -name "*.s" -exec cat {} \; | wc -l

FORCE:

# DO NOT DELETE
