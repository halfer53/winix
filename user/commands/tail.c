#include <stdio.h>
#include <stdlib.h>
#include <bsd/string.h>
#include <string.h>
#include <errno.h>

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
        }
    } else {
        for (j = 0; j < n; j++){
            printf("%s", lines[(i + j) % n]);
        }
    }
    free(line);
    for (j = 0; j < n; j++){
        free(lines[j]);
    }
    free(lines);
}

int main(int argc, char *argv[]){
    FILE* stream;
    int i = 0, n = 10;
    if (argc < 2){
        printf("Usage: tail [-n <num>] <file>");
        return 1;
    }
    for (i = 1; i < argc; i++){
        if(strcmp(argv[i], "-n") == 0){
            n = atoi(argv[i + 1]);
            i++;
        } else {
            stream = fopen(argv[i], "r");
            if (stream == NULL){
                fprintf(stderr, "Error opening file %s, %s", argv[1], strerror(errno));
                return 1;
            }
        }
    }
    print_last_n_lines(stream, n);
}