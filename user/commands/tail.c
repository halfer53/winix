#include <stdio.h>
#include <stdlib.h>
#include <bsd/string.h>

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
    if (argc < 2){
        printf("Usage: tail <file>");
        return 1;
    }
    stream = fopen(argv[1], "r");
    print_last_n_lines(stream, 10);
}