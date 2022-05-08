//
// Created by bruce on 19/04/20.
//
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <argp.h>
#include <dirent.h>
#include <kernel/proc.h>
#include <winix/welf.h>
#include <winix/list.h>
#include <winix/dev.h>
#include <fs/common.h>
#include <fs/cache.h>
#include <fs/super.h>
#include <fs/fs_methods.h>
#include "srec_import.h"
#include "../mock/mock.h"

void init_bitmap();

/* Program documentation. */
static char doc[] = "Generate FS Disk";

/* A description of the arguments we accept. */
static char args_doc[] = "-d -s [Source Path] -o [Output Path]";

/* The options we understand. */
static struct argp_option options[] = {
        {"offset",  't', "OFFSET1",      0,  "Srec Text Segment Offset" },
        {"output",   'o', "OUTPUT", 0, "Output Path" },
        {"source",   's', "SOURCE", 0, "Source Path" },
        {"unix time",   'u', "UNIX_TIME", 0, "Unix Time" },
        {0}
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
    char *output_path;
    char *source_path;
    int debug;
    int do_unit_test;
    unsigned int unix_time;
    int offset;
};

/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key)
    {
        case 't':
            arguments->offset = atoi(arg);
            break;
        case 'o':
            arguments->output_path = arg;
            break;
        case 's':
            arguments->source_path = arg;
            break;
        case 'u':
            arguments->unix_time = arg ? (unsigned int)strtoul(arg, NULL, 10) : 0;
            set_start_unix_time(arguments->unix_time);
            // printf("%ld\n", arguments->unix_time);
            break;

        default:
            return 0;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };


void write_disk(char* path){
    int i;
    char str2[] = "unsigned int DISK_RAW[] = {\n";
    char str3[] = "};\n";
    unsigned int *val = (unsigned int*)DISK_RAW;
    FILE *fp;

//    printf("opening %s\n", path);
    fp = fopen(path, "w");
    fprintf(fp, "%s", str2);

    for(i = 0; i < DISK_SIZE_DWORD; i++){
        fprintf(fp, "\t0x%08x,\n", *val++);
    }
    fprintf(fp, "%s\n\n", str3);
    fclose(fp);
}

void combine_srec_binary_debug(struct winix_elf_list* elf_list, struct srec_binary* binary, struct srec_debug* debug){
    struct winix_elf* elf = &elf_list->elf;
    elf->binary_pc = binary->binary_pc;
    elf->binary_size = binary->binary_idx;
    elf->bss_size = debug->bss_size;
    elf->data_size = debug->data_size;
    elf->text_size = debug->text_size;
    elf->binary_offset = binary->binary_offset;
    elf->magic = WINIX_ELF_MAGIC;
    elf_list->binary_data = binary->binary_data;
}

void merge_srec_debug(struct list_head* lists,
    struct list_head *srec_binary_list, struct list_head *srec_debug_list){
    struct srec_binary *b1, *b2;
    struct srec_debug *d1, *d2;
    char *name;
    list_for_each_entry_safe(struct srec_binary, b1, b2, srec_binary_list, list){
        name = b1->name;
        list_for_each_entry_safe(struct srec_debug, d1, d2, srec_debug_list, list){
            if(strcmp(name, d1->name) == 0){
                struct winix_elf_list *elf = malloc(sizeof(struct winix_elf_list));
                strlcpy(elf->name, b1->name, WINIX_ELF_NAME_LEN);
                combine_srec_binary_debug(elf, b1, d1);
                list_add(&elf->list, lists);
//                printf("found match for %s\n", name);
                list_del(&b1->list);
                free(b1);
                list_del(&d1->list);
                free(d1);
            }
        }
    }
}

void debug_super_block(char* str){
    struct superblock *sb = get_sb(get_dev(ROOT_DEV));
    printf("in use %d, remaining %d after %s\n", sb->s_block_inuse, sb->s_free_blocks, str);
}

#define PATH_LEN   (1024)


int get_srec_list(struct list_head *srec_list, char* path, int offset){
    struct list_head srec_binary_list;
    struct list_head srec_debug_list;
    struct dirent *dp;
    DIR *dfd;
    char filename_qfd[PATH_LEN] ;

    INIT_LIST_HEAD(&srec_binary_list);
    INIT_LIST_HEAD(&srec_debug_list);
    INIT_LIST_HEAD(srec_list);

    if ((dfd = opendir(path)) == NULL)
    {
        fprintf(stderr, "Can't open %s\n", path);
        return 0;
    }

    while ((dp = readdir(dfd)) != NULL)
    {
        if ( dp->d_type == DT_REG )
        {
#ifdef __wramp__
            char *dot = char32_index(dp->d_name, '.');
#else
            char *dot = index(dp->d_name, '.');
#endif
            char *extension_name = dot + 1;
            snprintf(filename_qfd, PATH_LEN, "%s/%s", path, dp->d_name);

            if(strcmp(extension_name, "srec") == 0){
                struct srec_binary *result = malloc(sizeof(struct srec_binary));
                decode_srec(filename_qfd, offset, result);
                list_add(&result->list, &srec_binary_list);
//                printf("srec %s %d %d pc %x\n", result->name, result->binary_offset, result->binary_idx, result->binary_pc);
            }else if(strcmp(extension_name, "verbose") == 0){
                struct srec_debug *result = malloc(sizeof(struct srec_debug));
                decode_srec_debug( filename_qfd, result);
                list_add(&result->list, &srec_debug_list);
//                printf("srec debug %s %x %x %x\n", result->name, result->text_size, result->data_size, result->bss_size);
            }
        }
    }
    merge_srec_debug(srec_list, &srec_binary_list, &srec_debug_list);
    return 0;
}

void write_srec_list(struct list_head* lists){
    static char bin_path[] = "/bin";
    struct winix_elf_list *pos, *tmp;
    char path[PATH_LEN];
    int ret, fd;
    int elf_size = sizeof(struct winix_elf);
    int binary_size;
    ret = sys_mkdir(curr_scheduling_proc, bin_path, 0x755);
    // printf("ret %d\n", ret);
    assert(ret == 0);
    list_for_each_entry_safe(struct winix_elf_list, pos, tmp, lists, list){
        snprintf(path, PATH_LEN, "%s%s%s", bin_path, "/", pos->name);
//        printf("writing %s %x %x\n", pos->name, pos->binary_data[0], pos->binary_data[1]);
        fd = sys_creat(curr_scheduling_proc, path, 0x755);
        assert(fd >= 0);

        ret = sys_write(curr_scheduling_proc, fd, &pos->elf, elf_size);
        assert(ret == elf_size);

        binary_size = TO_CHAR_SIZE_FROM_WRAMP(pos->elf.binary_size);
        ret = sys_write(curr_scheduling_proc, fd, pos->binary_data,  binary_size);
        assert(ret == binary_size);

        ret = sys_lseek(curr_scheduling_proc, fd, elf_size, SEEK_SET);
        assert(ret == elf_size);

        unsigned int read_buffer[pos->elf.binary_size];
        ret = sys_read(curr_scheduling_proc, fd, read_buffer, binary_size);
        assert(ret == binary_size);
        assert(memcmp(pos->binary_data, read_buffer, binary_size) == 0);
        
        ret = sys_close(curr_scheduling_proc, fd);
        assert(ret == 0);

    }
}

void verify_srec_with_disk(struct list_head* lists){
    static char bin_path[] = "/bin";
    struct winix_elf_list *pos, *tmp;
    char path[PATH_LEN];
    int ret, fd;
    int elf_size = sizeof(struct winix_elf);
    int binary_size;

    list_for_each_entry_safe(struct winix_elf_list, pos, tmp, lists, list){
        snprintf(path, PATH_LEN, "%s%s%s", bin_path, "/", pos->name);
        binary_size = TO_CHAR_SIZE_FROM_WRAMP(pos->elf.binary_size);
        struct winix_elf elf;

        fd = sys_open(curr_scheduling_proc, path, 0, 0);
        assert(fd >= 0);

        ret = sys_read(curr_scheduling_proc, fd, &elf, elf_size);
        assert(ret == elf_size);
        assert(memcmp(&elf, &pos->elf, elf_size) == 0);

        unsigned int read_buffer[pos->elf.binary_size];
        ret = sys_read(curr_scheduling_proc, fd, read_buffer,  binary_size);
        assert(ret == binary_size);
        assert(memcmp(read_buffer, pos->binary_data, binary_size) == 0);

        ret = sys_close(curr_scheduling_proc, fd);
        assert(ret == 0);

        // debug_super_block(pos->name);
        free(pos->binary_data);
        free(pos);
    }
}

int write_srec_to_disk(char* path, struct arguments* arguments){
    struct list_head srec_list;
    int ret;

    if((ret = get_srec_list(&srec_list, path, arguments->offset)))
        return ret;
    write_srec_list(&srec_list);
    verify_srec_with_disk(&srec_list);
    return 0;
}

/**
 * @brief write dummy text, e.g. abcd\nefgh\n
 * since wramp architecture sizeof(char) == sizeof(int)
 * we have to use int size in buffer when working with characters
 */
void write_dummy_file()
{
    
    int fd, ret, i, j, len;
    char c = 'a';
    int buffer[128];
    int *p = buffer;

    fd = sys_creat(curr_scheduling_proc, "/dummy.txt", 0x755);
    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 4; j++)
        {
            *p++ = (int)c++;
        }
        *p++ = '\n';
    }
    *p = '\0';
    // we need to do this convert int size to char size
    len = (p - buffer + 1) * sizeof(int);
    ret = sys_write(curr_scheduling_proc, fd, buffer, len);
    assert(ret == len);
    ret = sys_close(curr_scheduling_proc, fd);
    assert(ret == 0);
}

int main(int argc, char** argv){
    struct arguments arguments;
    memset(&arguments, 0, sizeof(struct arguments));

    arguments.debug = false;
    arguments.offset = 2048;
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    if( arguments.source_path == NULL && arguments.output_path == NULL ){
        fprintf(stderr,"Format ERROR: source file = NULL\n"
                       "output file = NULL\n"
                       "Offset %d\n",
                arguments.offset);
        return 1;
    }


    mock_init_proc();
    init_bitmap();
    init_disk();
    init_dev();
    init_fs();
    init_drivers();

    if(arguments.source_path && arguments.output_path){
        write_srec_to_disk(arguments.source_path, &arguments);
        write_dummy_file();
        flush_all_buffer();
        flush_inodes();
        flush_super_block(get_dev(ROOT_DEV));
        write_disk(arguments.output_path);
    }

//    do_tests();
//    printf("argc %d argv 0 %s \n", argc, argv[0]);
    return 0;
}
