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
export SREC_INCLUDE := include/srec
export includes := $(shell find include -name "*.h")
export SREC = $(shell find $(SREC_INCLUDE) -name "*.srec")
export TEXT_OFFSET := 2048
export INCLUDE_PATH := -I./include/posix_include -I./include
export CURR_UNIX_TIME := $(shell date +%s)
export WINIX_CFLAGS := -D__wramp__
export COMMON_CFLAGS := -DTEXT_OFFSET=$(TEXT_OFFSET) -D_DEBUG 
export CFLAGS := $(WINIX_CFLAGS) $(COMMON_CFLAGS)
export GCC_FLAG := -Wimplicit-fallthrough -Wsequence-point -Wswitch-default -Wswitch-unreachable \
		-Wswitch-enum -Wstringop-truncation -Wbool-compare -Wtautological-compare -Wfloat-equal \
		-Wshadow=global -Wpointer-arith -Wpointer-compare -Wcast-align -Wwrite-strings \
		-Wdangling-else -Wlogical-op -Wunused -Wpointer-to-int-cast -Wno-discarded-qualifiers \
		-Wno-builtin-declaration-mismatch -pedantic -Werror

# List of user libraries used by the kernel
KLIB_O = lib/syscall/wramp_syscall.o lib/ipc/ipc.o \
		lib/ansi/string.o lib/util/util.o lib/gen/ucontext.o lib/stdlib/atoi.o\
		lib/syscall/debug.o lib/posix/_sigset.o lib/ansi/rand.o lib/ansi/strl.o
L_HEAD = winix/limits/limits_head.o
L_TAIL = winix/limits/limits_tail.o
KERNEL_O = winix/*.o kernel/system/*.o kernel/*.o fs/*.o fs/system/*.o driver/*.o include/*.o
ALLDIR = init user kernel fs driver winix
ALLDIR_CLEAN = winix lib init user kernel fs driver include
FS_DEPEND = fs/*.c fs/system/*.c fs/mock/*.c winix/bitmap.c
UNIT_TEST_DEPEND = $(shell find tests -name "*.c" -not -name "utest_runner.c")

DISK = include/disk.c
UTEST_RUNNER = tests/utest_runner.c
START_TIME_FILE = include/startup_time.c
UNIT_TEST = unittest
FSUTIL = fsutil

all:
	$(Q)$(MAKE) fsutil
	$(Q)$(MAKE) buildlib
	$(Q)$(MAKE) kbuild
	$(Q)$(MAKE) $(DISK)
	$(Q)$(MAKE) include_build
	$(Q)wlink $(LDFLAGS) -Ttext 1024 -v -o winix.srec \
		$(L_HEAD) $(KERNEL_O) $(KLIB_O) $(L_TAIL) > $(SREC_INCLUDE)/winix.verbose
ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t winix.srec"
endif

$(UTEST_RUNNER): $(UNIT_TEST_DEPEND)
	$(Q)python3 tools/utest_generator.py $(UNIT_TEST_DEPEND) > $(UTEST_RUNNER)

$(UNIT_TEST): $(FS_DEPEND) $(UNIT_TEST_DEPEND) $(UTEST_RUNNER)
ifeq ($(KBUILD_VERBOSE),0)
	@echo "CC \t $(UNIT_TEST)"
endif
	$(Q)gcc -g -DFSUTIL $(GCC_FLAG) $(COMMON_CFLAGS) -I./include/fs_include -I./include $^ -o $(UNIT_TEST)

test: $(UNIT_TEST)
	$(Q)./$(UNIT_TEST)

wsh: user/bash/*.c lib/ansi/strl.c
	$(Q)gcc -g -DFSUTIL $(GCC_FLAG) $(COMMON_CFLAGS) $^ -o wsh

$(FSUTIL): $(FS_DEPEND) fs/fsutil/*.c 
ifeq ($(KBUILD_VERBOSE),0)
	@echo "CC \t $(FSUTIL)"
endif
	$(Q)gcc -g -DFSUTIL $(GCC_FLAG) $(COMMON_CFLAGS) -I./include/fs_include -I./include $^ -o $(FSUTIL)

buildlib:
	$(Q)$(MAKE) $(build)=lib

kbuild: $(ALLDIR)
$(ALLDIR): FORCE
	$(Q)$(MAKE) $(build)=$@

$(DISK): $(SREC) fsutil
ifeq ($(KBUILD_VERBOSE),0)
	@echo "LD \t disk.c"
endif
	$(Q)./fsutil -t $(TEXT_OFFSET) -o $(DISK) -s $(SREC_INCLUDE) -u $(CURR_UNIX_TIME)
	
include_build:
	$(Q)echo "unsigned int start_unix_time=$(CURR_UNIX_TIME);\n" > $(START_TIME_FILE)
	$(Q)$(MAKE) $(build)=include

clean:
	$(Q)rm -f $(FSUTIL)
	$(Q)rm -f $(UNIT_TEST)
	$(Q)rm -f $(UTEST_RUNNER)
	$(Q)rm -f $(START_TIME_FILE)
	$(Q)rm -f $(DISK)
	$(Q)$(MAKE) $(cleanall)='$(ALLDIR_CLEAN)'

stat:
	@echo "C Lines: "
	@find . -type d -name "include" -prune -o -name "*.c"  -exec cat {} \; | wc -l
	@echo "Header LoC: "
	@find . -name "*.h" -exec cat {} \; | wc -l
	@echo "Assembly LoC: "
	@find . -name "*.s" -exec cat {} \; | wc -l

FORCE:

# DO NOT DELETE
