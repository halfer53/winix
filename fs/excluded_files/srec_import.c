#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <libgen.h>
#include <argp.h>
#include <winix/welf.h>

struct srec_binary{
    size_t binary_pc;
    size_t binary_offset;
    size_t binary_size;
    size_t binary_idx;
    unsigned int* binary_data;
};

int decode_srec(char *filename, int offset, struct srec_binary* result);
void init_srec_binary_struct(struct srec_binary* result);
int winix_load_srec_words_length(char *line);
int winix_load_srec_mem_val(char *line,  const char* filename, struct srec_binary* result);
#define TO_UPPER_CHAR(c) (c - 32)

const char *argp_program_version =
  "argp-ex3 1.0";
const char *argp_program_bug_address =
  "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
static char doc[] =
  "Generate Winix ELF binary based on srec file";

/* A description of the arguments we accept. */
static char args_doc[] = "-t [offset] -f [srec file] -d [debug file]";

/* The options we understand. */
static struct argp_option options[] = {
  {"offset",  't', "OFFSET1",      0,  "Srec Text Segment Offset" },
  {"srec",   'f', "FILE", 0, "Srec File" },
  {"debug",   'd', "DEBUG", 0, "Srec Debug Output" }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
  char *srec_file;
  char *debug_file;
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
    case 'f':
        arguments->srec_file = arg;
        break;
    case 'd':
        arguments->debug_file = arg;
        break;

    default:
      return 0;
    }
  return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };
//
//int main(int argc, char *argv[])
//{
//    char cwd[PATH_MAX];
//    int i = 1;
//    int offset = 2048;
//    struct srec_binary srec_result;
//    struct winix_elf elf_result;
//    struct arguments arguments;
//    memset(&arguments, 0, sizeof(struct arguments));
//    init_srec_binary_struct(&srec_result);
//
//    arguments.debug_file = "";
//    arguments.srec_file = "";
//    /* Parse our arguments; every option seen by parse_opt will
//        be reflected in arguments. */
//    argp_parse (&argp, argc, argv, 0, 0, &arguments);
//    getcwd(cwd, PATH_MAX);
//    printf ("srec file = %s\n"
//            "debug file = %s\n"
//            "offset %d\n"
//            "cwd %s\n",
//            arguments.srec_file, arguments.debug_file, arguments.offset, cwd);
//
//    srec_result.binary_offset = arguments.offset;
//    decode_srec(arguments.srec_file, arguments.offset, &srec_result);
//    return 0;
//}

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
    printf("%ld %x\n", srec_result->binary_idx, item);
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

int decode_srec(char *path, int offset, struct srec_binary* result)
{

    int i = 0;
    int length = 0;
    int wordslength = 0;
    int temp = 0;
    int recordtype = 0;
    int success = 0;
    int counter = 0;
    size_t *memory_values;
    size_t wordsLoaded = 0;
    FILE *fp;
    char *line = NULL;
    size_t len = 10240;
    size_t read;
    char *upperfilename, *filename;

    printf("path = '%s'\n", path);
    fp = fopen(path, "rw");
    if (fp == NULL){
        perror("Path cannot be found ");
        exit(EXIT_FAILURE);
    }
    line = malloc(len);
    
    filename = remove_extension(path);
    printf("filename is '%s' path = '%s'\n", filename, path);
//    upperfilename = malloc(strlen(filename) + 1);
//    toUpperCase(upperfilename, filename);
    // printf("#ifndef _%s_H_\n", upperfilename);
    // printf("#define _%s_H_\n", upperfilename);
    // printf("unsigned int %s_code[] = {\n", filename);

    while(fgets(line, len, fp)){
        temp = winix_load_srec_mem_val(line, filename, result);
        wordsLoaded += temp;
    };
    free(line);

    result->binary_offset = offset;

    print_srec_result(result, filename);
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
        if (a[i] <= 57)
        {
            val += (a[i] - 48) * (1 << (4 * (len - 1 - i)));
        }

        else
        {
            val += (a[i] - 55) * (1 << (4 * (len - 1 - i)));
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


int winix_load_srec_mem_val(char *line,  const char* filename, struct srec_binary* result)
{
    int wordsLoaded = 0;
    int index = 0;
    int checksum = 0;
    char byteCheckSum = 0;
    int recordType = 0;
    int addressLength = 0;
    int byteCount = 0;
    char buffer[128];
    char tempBufferCount = 0;
    int address = 0;
    char data[255];
    char readChecksum = 0;
    int datalength = 0;
    size_t memVal = 0;
    int i = 0;
    int j = 0;
    static int debug = 1;

     printf("decoding %s\n",line);
    // printf("loop %d\n",linecount );
    // Start code, always 'S'
    if (line[index++] != 'S')
    {
        printf("Expection S\n");
        goto final;
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
        goto final;
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
    readChecksum = (char) (hex2int(buffer, tempBufferCount) & 0xff);

    byteCheckSum = (char)(~checksum & 0xFF);
    // printf("checksum %x\r\n",byteCheckSum );
    if (readChecksum != byteCheckSum)
    {
        printf("failed checksum %x %x on line %s\r\n", readChecksum, byteCheckSum, line);
        goto final;
    }

    // Put in memory
    if ((byteCount - 1) % 4 != 0)
    {
        printf("Data should only contain full 32-bit words.\n");
        goto final;
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

                // memory_values[start_index + wordsLoaded] = memVal;
                srec_add_binary(result, memVal);
                wordsLoaded++;
                // printf("0x%08x,\n", (unsigned int)memVal);
            }
            break;

        case 7: // entry point for the program.
            // printf("\n};\n#define %s_pc\t0x%08x\n", filename, (unsigned int)address);
            result->binary_pc = address;
            wordsLoaded++;
    }

    final:
    return wordsLoaded;

}




int winix_load_srec_words_length(char *line)
{
    int i = 0;

    int index = 0;
    int checksum = 0;
    char byteCheckSum = 0;
    int recordType = 0;
    int byteCount = 0;
    char buffer[128];
    char tempBufferCount = 0;
    int wordsCount = 0;
    int length = 0;
    int readChecksum = 0;
    int data = 0;

    index = 0;
    checksum = 0;
    // printf("loop %d\n",linecount );
    // Start code, always 'S'
    if (line[index++] != 'S')
    {
        printf("Expection S\n");
    }

    recordType = line[index++] - '0';
    if (recordType != 6)
    {
        printf("recordType %d\n", recordType);
        printf("format is incorrect\n");
        return 0;
    }
    tempBufferCount = substring(buffer, line, index, 2);
    // printf("record value %s, value in base 10: %d,length %d\r\n",buffer,hex2int(buffer,tempBufferCount),tempBufferCount);
    byteCount = hex2int(buffer, tempBufferCount);
    index += 2;
    checksum += byteCount;
    tempBufferCount = substring(buffer, line, index, (byteCount - 1) * 2);
    // printf("temp byte value %s, value in base 10: %d,length %d\r\n",buffer,hex2int(buffer,tempBufferCount),tempBufferCount);
    data = hex2int(buffer, tempBufferCount);
    // printf("data %d\n", data);
    index += (byteCount - 1) * 2;
    checksum += data;

    // Checksum, two hex digits. Inverted LSB of the sum of values, including byte count, address and all data.
    // readChecksum = (byte)Convert.ToInt32(line.substring(index, 2), 16);
    // printf("checksum %d\n",checksum );
    tempBufferCount = substring(buffer, line, index, 2);
    // printf("read checksum value %s, value in base 10: %d,length %d\r\n",buffer,hex2int(buffer,tempBufferCount),tempBufferCount);
    readChecksum = hex2int(buffer, tempBufferCount);
    // printf("readChecksum %d\n",readChecksum );
    // printf("checksum %d\n",checksum );
    // printf("checksum %d\r\n",checksum );
    if (checksum > 255)
    {
        byteCheckSum = (char)(checksum & 0xFF);
        // printf("checksum %d\r\n",byteCheckSum );
        byteCheckSum = ~byteCheckSum;
    }
    else
    {
        byteCheckSum = ~byteCheckSum;
        byteCheckSum = checksum;
    }
    // printf("checksum %d\r\n",byteCheckSum );
    if (readChecksum != byteCheckSum)
    {
        printf("failed checksum\r\n");
        return 0;
    }
    return data;
}
