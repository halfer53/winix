//
// Created by bruce on 19/04/20.
//
#define __STDC_WANT_LIB_EXT1__ 1
#include <fs/common.h>
#include <fs/cache.h>
#include <kernel/proc.h>
#include <fs/super.h>
#include "makefs_only/cmake_util.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <argp.h>
#include <stdlib.h>
#include <winix/welf.h>
#include <dirent.h>
#include <fs/fs_methods.h>
#include <winix/list.h>
#include <winix/dev.h>
#include <fs/super.h>
#include "makefs_only/srec_import.h"

void init_bitmap();

char DISK_RAW[DISK_SIZE];

/* Program documentation. */
static char doc[] = "Generate FS Disk";

/* A description of the arguments we accept. */
static char args_doc[] = "-d -s [Source Path] -o [Output Path]";

/* The options we understand. */
static struct argp_option options[] = {
        {"offset",  't', "OFFSET1",      0,  "Srec Text Segment Offset" },
        {"debug",  'd', 0,      OPTION_ARG_OPTIONAL,  "Is Debugging" },
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
            arguments->offset = arg ? atoi(arg) : 2048;
            break;
        case 'd':
            arguments->debug = true;
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
    char curr_dir[100];
    int i;
    char filename[] = "disk.c";
    char str2[] = "unsigned int DISK_RAW[] = {\n";
    char str3[] = "};\n";
    unsigned int *val = (unsigned int*)DISK_RAW;
    FILE *fp;
    getcwd(curr_dir, 100);

//    printf("opening %s\n", path);
    fp = fopen(path, "w");
    fprintf(fp, "%s", str2);

    for(i = 0; i < DISK_SIZE_WORD; i++){
        fprintf(fp, "\t0x%08x,\n", *val++);
    }
    fprintf(fp, "%s\n\n", str3);
    fclose(fp);
}

void init_disk(){
    int ret = makefs(DISK_RAW, DISK_SIZE);
    if(ret){
        printf("makefs failed");
        _exit(1);
    }
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

void combine_srec_binary_and_debug_list(struct list_head* lists,
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

#define PATH_LEN   (256)

void write_srec_list(struct list_head* lists){
    static char bin_path[] = "/bin";
    struct winix_elf_list *pos, *tmp;
    char path[PATH_LEN];
    int ret, fd, size;
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
        binary_size = TO_CHAR_SIZE(pos->elf.binary_size);
        ret = sys_write(curr_scheduling_proc, fd, pos->binary_data,  binary_size);
        assert(ret == binary_size);
        ret = sys_close(curr_scheduling_proc, fd);
        assert(ret == 0);
        list_del(&pos->list);
        // debug_super_block(pos->name);
        free(pos);
    }
    flush_all_buffer();
    flush_super_block(get_dev(ROOT_DEV));
}

int write_srec_to_disk(char* path, struct arguments* arguments){
    struct list_head srec_binary_list;
    struct list_head srec_debug_list;
    struct list_head srec_list;
    struct dirent *dp;
    DIR *dfd;
    char filename_qfd[256] ;

    INIT_LIST_HEAD(&srec_binary_list);
    INIT_LIST_HEAD(&srec_debug_list);
    INIT_LIST_HEAD(&srec_list);

    if ((dfd = opendir(path)) == NULL)
    {
        fprintf(stderr, "Can't open %s\n", path);
        return 0;
    }

    while ((dp = readdir(dfd)) != NULL)
    {
        if ( dp->d_type == DT_REG )
        {
            char *dot = strchr(dp->d_name, '.');
            char *extension_name = dot + 1;
            sprintf(filename_qfd,"%s/%s", path, dp->d_name);

            if(strcmp(extension_name, "srec") == 0){
                struct srec_binary *result = malloc(sizeof(struct srec_binary));
                decode_srec(filename_qfd, arguments->offset, result);
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
    combine_srec_binary_and_debug_list(&srec_list, &srec_binary_list, &srec_debug_list);
    write_srec_list(&srec_list);
    return 0;
}

int main(int argc, char** argv){
    struct arguments arguments;
    memset(&arguments, 0, sizeof(struct arguments));

    arguments.debug = false;
    arguments.offset = 2048;
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    if( arguments.source_path == NULL && arguments.output_path == NULL && arguments.debug == false){
        fprintf(stderr,"Format ERROR: source file = %s\n"
                       "output file = %s\n"
                       "is debug %d\n"
                       "Offset %d\n",
                arguments.source_path, arguments.output_path, arguments.debug, arguments.offset);
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
        write_disk(arguments.output_path);
    }else if(arguments.debug){
        unit_test1();
    }

//    do_tests();
//    printf("argc %d argv 0 %s \n", argc, argv[0]);
    return 0;
}