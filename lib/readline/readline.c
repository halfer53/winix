#include <readline/readline.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFER_SIZ  (128)

static bool rl_initied = false;

void init_readline(){
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);
}

int rl_readchar(){
    char c;
    int ret = read(STDIN_FILENO, &c, 1);
    if(ret == 1)
        return c;
    return -1;
}

int rl_getline(){
    int c;
    while(1){
        c = rl_readchar();
        if(c == EOF)
            return -1;
        if(c == '\n'){
            rl_line_buffer[rl_end] = '\0';
            rl_done = 1;
            return 0;
        }
        else if (c == '\b' || c == 127){
            if(rl_point > 0){
                rl_point--;
                rl_end--;
                fwrite("\b", 1, sizeof(char), stdout);
                fflush(stdout);
            }
            continue;
        }
        
        if (rl_end >= BUFFER_SIZ)
            return -1;
        rl_point++;
        rl_line_buffer[rl_end++] = c;
    }
    rl_line_buffer[rl_end] = '\0';
    return 0;
}

char *readline(char *prompt){
    int ret;
    if(!rl_initied){
        init_readline();
        rl_initied = true;
    }

    rl_prompt = prompt;
    rl_point = 0;
    rl_end = 0;
    rl_done = 0;
    rl_line_buffer = malloc(BUFFER_SIZ);

    fwrite(rl_prompt, 1, strlen(rl_prompt), stdout);
    fflush(stdout);
    
    ret = rl_getline();
    if (ret && !rl_end)
        return NULL;
    return rl_line_buffer;
}