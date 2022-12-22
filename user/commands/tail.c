#include <stdio.h>
#include <stdlib.h>
#include <bsd/string.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

void print_last_n_lines(FILE* stream, int n){
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_count = 0;
    int i = 0, j = 0;
    char** lines = (char**)malloc(n * sizeof(char*));

    while ((read = getline(&line, &len, stream)) != -1) {
        int line_len = len * sizeof(char) + 1;
        if (line_count >= n)
            free(lines[i]);
        
        lines[i] = (char*)malloc(line_len);
        strlcpy(lines[i], line, line_len);
        i = (i + 1) % n;
        line_count++;
    }
    if (line_count < n){
        for (j = 0; j < line_count; j++){
            printf("%s", lines[j]);
            free(lines[j]);
        }
    } else {
        for (j = 0; j < n; j++){
            char *_line = lines[(i + j) % n];
            printf("%s", _line);
            free(_line);
        }
    }
    free(lines);
}

int main(int argc, char *argv[]){
    FILE* stream = stdin;
    int i = 0, n = 10;
    bool follow = false;
    struct timespec ts;
    char *endptr;

    for (i = 1; i < argc; i++){
        if(strcmp(argv[i], "-n") == 0){
            n = strtol(argv[i + 1], &endptr, 10);
            if (*endptr){
                fprintf(stderr, "Invalid number of lines: %s\n", argv[i + 1]);
                return 1;
            }
            i++;
        } else if( strcmp(argv[i], "-f") == 0){
            follow = true;
        } else {
            stream = fopen(argv[i], "r");
            if (stream == NULL){
                fprintf(stderr, "Error opening file %s, %s\n", argv[1], strerror(errno));
                return 1;
            }
        }
    }
    
    print_last_n_lines(stream, n);

    if (follow){
        int ret;
        char buffer[64];
        memset(&ts, 0, sizeof(struct timespec));
        ts.tv_nsec = 125000000;
        while (true){
            nanosleep(&ts, NULL);
            while((ret = fread(buffer, 1, sizeof(buffer), stream)) > 0){
                fwrite(buffer, 1, ret, stdout);
            }
        }
    }
}