#include <readline/readline.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZ  (128)

static bool rl_initied = false;
struct termios _rl_termios;
struct termios* rl_termios = &_rl_termios;

void init_readline(){
    cc_t *cc;
    tcgetattr(STDIN_FILENO, rl_termios);
    rl_termios->c_lflag &= ~(ICANON | ECHO);
    cc = rl_termios->c_cc;
    cc[VMIN] = 1;
    cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, rl_termios);
    rl_initied = true;
}

int rl_readchar(){
    char c;
    int ret = read(STDIN_FILENO, &c, 1);
    if(ret == 1)
        return c;
    return -1;
}

int rl_getline(){
    int c = 0;

    rl_point = 0;
    rl_end = 0;
    rl_done = 0;
    rl_line_buffer = calloc(sizeof(char), BUFFER_SIZ);
    while(1){
        c = rl_readchar();
        if (c == '\r')
            c = '\n';
        if(c == '\n'){
            rl_line_buffer[rl_end] = '\0';
            rl_done = 1;
            return 0;
        }
        else if (c == rl_termios->c_cc[VERASE]){ // Backspace or Delete
            if(rl_point > 0){
                rl_point--;
                rl_end--;
                printf("%c", 8);
                fflush(stdout);
            }
            continue;
        }
        else if (c == EOF || c == rl_termios->c_cc[VEOF]) { // Control D
            return EOF;
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
    if(!rl_initied)
        init_readline();

    rl_prompt = prompt;
    fwrite(rl_prompt, 1, strlen(rl_prompt), stdout);
    fflush(stdout);
    
    ret = rl_getline();
    if (ret)
        return NULL;
    return rl_line_buffer;
}