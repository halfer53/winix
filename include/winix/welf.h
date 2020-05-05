#ifndef _WINIX_WELF_H_
#define _WINIX_WELF_H_

#include <sys/types.h>

#define WELF_MAGIC  (0x19960503) // my birthday lol

struct winix_elf{
    unsigned int magic;
    size_t binary_pc;
    size_t binary_offset;
    size_t binary_size;
    size_t text_size;
    size_t data_size;
    size_t bss_size;
    unsigned int* binary_data;
};

#endif