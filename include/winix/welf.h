#ifndef _WINIX_WELF_H_
#define _WINIX_WELF_H_

#include <sys/types.h>
#include <winix/type.h>

#define WINIX_ELF_MAGIC  (0x19960503) // my birthday lol
#define WINIX_ELF_NAME_LEN  (256)

struct winix_elf{
    unsigned int magic;
    unsigned int binary_pc;
    unsigned int binary_offset;
    unsigned int binary_size;
    unsigned int text_size;
    unsigned int data_size;
    unsigned int bss_size;
};


struct srec_binary{
    char name[WINIX_ELF_NAME_LEN];
    unsigned int binary_pc;
    unsigned int binary_offset;
    unsigned int binary_size;
    unsigned int binary_idx;
    unsigned int* binary_data;
    struct list_head list;
};

struct srec_debug{
    char name[WINIX_ELF_NAME_LEN];
    unsigned int text_size;
    unsigned int data_size;
    unsigned int bss_size;
    struct list_head list;
};

struct winix_elf_list{
    char name[WINIX_ELF_NAME_LEN];
    struct winix_elf elf;
    unsigned int* binary_data;
    struct list_head list;
};

#endif

