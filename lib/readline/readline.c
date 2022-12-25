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
    tcgetattr(STDIN_FILENO, rl_termios);
    rl_termios->c_lflag &= ~(ICANON | ECHO);
    rl_termios->c_cc[VMIN] = 1;
    rl_termios->c_cc[VTIME] = 0;
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
    rl_line_buffer = malloc(sizeof(char) * BUFFER_SIZ);
    while(1){
        c = rl_readchar();
        if (c == '\r')
            c = '\n';
        
        if (c == rl_termios->c_cc[VERASE]){ // Backspace or Delete
            if(rl_point > 0){
                char erase = 8;
                rl_point--;
                rl_end--;
                fwrite(&erase, 1, sizeof(char), rl_outstream);
                fflush(rl_outstream);
            }
            continue;
        }
        else if (c == EOF || c == rl_termios->c_cc[VEOF]) { // Control D
            break;
        }
        
        if (rl_end >= BUFFER_SIZ)
            break;
        
        rl_point++;
        rl_line_buffer[rl_end++] = c;
        fwrite(&c, 1, sizeof(char), rl_outstream);

        if (c == '\n')
            break;
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

    if (rl_initied)
        tcsetattr(STDIN_FILENO, TCSANOW, rl_termios);
    
    ret = rl_getline();
    if (ret)
        return NULL;
    return rl_line_buffer;
}