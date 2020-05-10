//
// Created by bruce on 6/05/20.
//

#ifndef FS_SREC_IMPORT_H
#define FS_SREC_IMPORT_H

int decode_srec(char *filename, int offset, struct srec_binary* result);
void init_srec_binary_struct(struct srec_binary* result);
int winix_load_srec_words_length(char *line);
int winix_load_srec_mem_val(char *line, struct srec_binary* result);
int hex2int(char *a, int len);
int decode_srec_debug(char* filepath, struct srec_debug* result);

#endif //FS_SREC_IMPORT_H
