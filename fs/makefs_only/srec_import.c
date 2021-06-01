#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <libgen.h>
#include <argp.h>
#include <winix/welf.h>
#include "srec_import.h"

#define TO_UPPER_CHAR(c) (c - 32)

void print_srec_result(struct srec_binary* result, char* filename){
    printf("unsigned int %s_code[] = {\n", filename);
    for(int j = 0; j < result->binary_idx; j++){
        printf("0x%08x,\n", (unsigned int)result->binary_data[j]);
    }
    printf("};\n");
}

char* remove_extension(char *str)
{
    char *lastdot, *mystr;
    mystr = basename(str);
    if (mystr == NULL)
        return NULL;
    lastdot = strrchr(mystr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    return mystr;
}

int toUpperCase(char *to, char *src)
{
    while (*src)
    {
        *to++ = islower(*src) ? TO_UPPER_CHAR(*src) : *src;
        src++;
    }
    return 0;
}



int srec_add_binary(struct srec_binary* srec_result, unsigned int item){
//    printf("%ld %x\n", srec_result->binary_idx, item);
    srec_result->binary_data[srec_result->binary_idx] = item;
    srec_result->binary_idx++;
    if(srec_result->binary_idx >= srec_result->binary_size){
        srec_result->binary_size += 1024;
        srec_result->binary_data = realloc(srec_result->binary_data, sizeof(unsigned int) * srec_result->binary_size);
        if(srec_result->binary_data == NULL){
            perror("realloc returned NULL ");
            abort();
        }
    }
    return 0;
}

void init_srec_binary_struct(struct srec_binary* result){
    memset(result, 0, sizeof(struct srec_binary));
    result->binary_size = 1024;
    result->binary_data = (unsigned int*)calloc(result->binary_size, sizeof(unsigned int) );
}

static char text_size[] = ".text segment size = 0x";
static char data_size[] = ".data segment size = 0x";
static char bss_size[] = ".bss segment size = 0x";

int decode_segment_size(size_t* val, char* prefix, char* line){
    int len = strlen(prefix);
    char *strvalue = &line[len];
    if(strvalue[strlen(strvalue) - 1] == '\n'){
        strvalue[strlen(strvalue) - 1] = '\0';
    }
    while(*strvalue && *strvalue == '0'){
        strvalue++;
    }
    *val = hex2int(strvalue, strlen(strvalue));
//    printf(" size %s | %d %x\n",  strvalue, *val, *val);
    return 0;
}

int decode_srec_debug(char* filepath, struct srec_debug* result){
    FILE* f = fopen(filepath, "r");
    char *filename = remove_extension(filepath);
    if(!f)
        return -1;
    char *line = malloc(1024);
    if(!line){
        fclose(f);
        return -1;
    }
    strlcpy(result->name, filename, WINIX_ELF_NAME_LEN);
    while(fgets(line, 1024, f)){
        if(strstr(line, text_size)){
            decode_segment_size(&result->text_size, text_size, line);
        }
        else if(strstr(line, data_size)){
            decode_segment_size(&result->data_size, data_size, line);
        }
        else if(strstr(line, bss_size)){
            decode_segment_size(&result->bss_size, bss_size, line);
        }
    }
    free(line);
    return 0;
}

int decode_srec(char *path, int offset, struct srec_binary* result)
{
    int temp = 0;
    size_t *memory_values;
    size_t wordsLoaded = 0;
    FILE *fp;
    char *line = NULL;
    size_t len = 10240;
    char *filename;
    init_srec_binary_struct(result);
    fp = fopen(path, "r");
    if (fp == NULL){
        perror("Path cannot be found ");
        exit(EXIT_FAILURE);
    }
    line = malloc(len);
    filename = remove_extension(path);
//    printf("filename is '%s' path = '%s'\n", filename, path);
    strlcpy(result->name, filename, WINIX_ELF_NAME_LEN);
    while(fgets(line, len, fp)){
        temp = winix_load_srec_mem_val(line, result);
        wordsLoaded += temp;
    };
    free(line);

    result->binary_offset = offset;
//    print_srec_result(result, filename);
    return 0;
}

void assert(int expression, const char *message)
{
    if (!expression)
    {
        printf("\r\nAssertion Failed ");
        printf("%s\r\n", message);
        abort();
    }
}
int hex2int(char *a, int len)
{
    int i;
    int val = 0;

    for (i = 0; i < len; i++)
    {
        if (a[i] <= '9')
        {
            val += (a[i] - 48) * (1 << (4 * (len - 1 - i)));
        }

        else if (a[i] <= 'F')
        {
            val += (a[i] - 55) * (1 << (4 * (len - 1 - i)));
        }
        else if(a[i] <= 'f'){
            val += (a[i] - 87) * (1 << (4 * (len - 1 - i)));
        }
    }

    return val;
}

int substring(char *buffer, char *original, int start_index, int length)
{
    int i = 0;
    int count = 0;
    for (i = start_index; i < length + start_index; i++)
    {
        buffer[count] = original[i];
        count++;
    }
    buffer[count] = '\0';
    return count;
}

int winix_load_srec_mem_val(char *line, struct srec_binary* result)
{
    int wordsLoaded = 0;
    int index = 0;
    int checksum = 0;
    byte byteCheckSum = 0;
    int recordType = 0;
    int addressLength = 0;
    int byteCount = 0;
    char buffer[128];
    char tempBufferCount = 0;
    int address = 0;
    byte data[255];
    int readChecksum = 0;
    int datalength = 0;
    size_t memVal = 0;
    int i = 0;
    int j = 0;
    static int debug = 1;

    // printf("%s\r\n",line);
    // printf("loop %d\n",linecount );
    // Start code, always 'S'
    if (line[index++] != 'S')
    {
        printf("Expection S\n");
        goto err_end;
    }

    // Record type, 1 digit, 0-9, defining the data field
    //0: Vendor-specific data
    //1: 16-bit data sequence
    //2: 24 bit data sequence
    //3: 32-bit data sequence
    //5: Count of data sequences in the file. Not required.
    //7: Starting address for the program, 32 bit address
    //8: Starting address for the program, 24 bit address
    //9: Starting address for the program, 16 bit address
    recordType = line[index++] - '0';

    switch (recordType)
    {
        case 0:
        case 1:
        case 9:
            addressLength = 2;
            break;

        case 5:
        case 6:
            addressLength = 0;
            break;

        case 2:
        case 8:
            addressLength = 3;
            break;

        case 3:
        case 7:
            addressLength = 4;
            break;

        default:
            printf("unknown record type\n");
            goto err_end;
    }

    tempBufferCount = substring(buffer, line, index, 2);
    // printf("record value %s, value in base 10: %d,length %d\r\n",buffer,hex2int(buffer,tempBufferCount),tempBufferCount);
    byteCount = hex2int(buffer, tempBufferCount);
    index += 2;
    checksum += byteCount;

    // byteCount = ((int)line[index++])*10 + ((int)line[index++]);
    // int byteCount = Convert.ToInt32(line.substring(index, 2), 16);
    // printf("byteCount %d\r\n",byteCount);

    // Address, 4, 6 or 8 hex digits determined by the record type
    for (i = 0; i < addressLength; i++)
    {
        tempBufferCount = substring(buffer, line, index + i * 2, 2);
        // printf("temp byte value %s, value in base 10: %d,length %d\r\n",buffer,hex2int(buffer,tempBufferCount),tempBufferCount);
        checksum += hex2int(buffer, tempBufferCount);
        // string ch = line.substring(index + i * 2, 2);
        // checksum += Convert.ToInt32(ch, 16);
    }
    if (addressLength != 0)
    {
        tempBufferCount = substring(buffer, line, index, addressLength * 2);
        // printf("temp address value %s, value in base 10: %d,length %d\r\n",buffer,hex2int(buffer,tempBufferCount),tempBufferCount);
        address = hex2int(buffer, tempBufferCount);
    }

    // address = Convert.ToInt32(line.substring(index, addressLength * 2), 16);
    // printf("index %d\n",index );
    index += addressLength * 2;
    // printf("index %d\n",index );
    byteCount -= addressLength;
    // printf("byteCount %d\n",byteCount );
    // Data, a sequence of bytes.
    // data.length = 255

    for (i = 0; i < byteCount - 1; i++)
    {
        tempBufferCount = substring(buffer, line, index, 2);
        // printf("temp byte value %s, value in base 10: %d,length %d\r\n",buffer,hex2int(buffer,tempBufferCount),tempBufferCount);
        data[i] = hex2int(buffer, tempBufferCount);
        // data[i] = (byte)Convert.ToInt32(line.substring(index, 2), 16);
        index += 2;
        checksum += data[i];
    }

    // Checksum, two hex digits. Inverted LSB of the sum of values, including byte count, address and all data.
    // readChecksum = (byte)Convert.ToInt32(line.substring(index, 2), 16);

    tempBufferCount = substring(buffer, line, index, 2);
    // printf("read checksum value %s, value in base 10: %d,length %d\r\n",buffer,hex2int(buffer,tempBufferCount),tempBufferCount);
    readChecksum = hex2int(buffer, tempBufferCount);
    // printf("checksum %d\r\n",checksum );
    byteCheckSum = (byte)(checksum & 0xFF);
    // printf("checksum %d\r\n",byteCheckSum );
    byteCheckSum = ~byteCheckSum;
    // printf("checksum %d\r\n",byteCheckSum );
    if (readChecksum != byteCheckSum)
    {
        printf("failed checksum\r\n");
        goto err_end;
    }

    // Put in memory
    if ((byteCount - 1) % 4 != 0)
    {
        printf("Data should only contain full 32-bit words.\n");
    }

    // printf("recordType %d\n", recordType);
    // printf("%lu\n",(size_t)data[0] );
    // printf("byteCount %d\n",byteCount );
    switch (recordType)
    {
        case 3: // data intended to be stored in memory.

            for (i = 0; i < byteCount - 1; i += 4)
            {
                memVal = 0;
                for (j = i; j < i + 4; j++)
                {

                    memVal <<= 8;
                    memVal |= data[j];
                }

                wordsLoaded++;
                // memory_values[start_index + wordsLoaded] = memVal;
//                printf("0x%08x, %08x\n", (unsigned int)memVal, memVal);
                srec_add_binary(result, memVal);
            }
            break;

        case 7: // entry point for the program.
            result->binary_pc = (unsigned int)address;
            return 0;
    }

    return wordsLoaded;
    err_end:
    return 0;

}
