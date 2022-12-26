#include <readline/readline.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <bsd/string.h>
#include <readline/history.h>

#define BUFFER_SIZ  (128)
#define CTRL_P  (16)
#define CTRL_N  (14)

static bool rl_initied = false;
struct termios _rl_termios;
struct termios* rl_termios = &_rl_termios;

void init_readline(){
    tcgetattr(STDIN_FILENO, rl_termios);
    rl_termios->c_lflag &= ~(ICANON | ECHO);
    rl_termios->c_cc[VMIN] = 1;
    rl_termios->c_cc[VTIME] = 0;
    rl_initied = true;
}

int rl_readchar(){
    char c;
    int ret = read(STDIN_FILENO, &c, 1);
    if(ret == 1)
        return c;
    return -1;
}

void rl_backspace(){
    char erase = 8;
    rl_point--;
    rl_end--;
    fwrite(&erase, 1, sizeof(char), rl_outstream);
    fflush(rl_outstream);
}

void rl_clear(){
    while(rl_end > 0){
        rl_backspace();
    }
}

static void navigate_history(HIST_ENTRY *func (void)){
    size_t len;
    char *line;
    HIST_ENTRY* entry = func();
    rl_clear();
    if (!entry)
        return;

    line = entry->line;
    len = strlen(line);

    if (line[len - 1] == '\n'){
        line[--len] = '\0';
    }
    
    fwrite(line, 1, len, rl_outstream);
    fflush(rl_outstream);

    strlcpy(rl_line_buffer, line, BUFFER_SIZ);
    rl_end = len;
    rl_point = len;
}

int rl_getline(){
    int c = 0;

    rl_point = 0;
    rl_end = 0;
    rl_done = 0;
    rl_eof_found = 0;
    rl_line_buffer = malloc(sizeof(char) * BUFFER_SIZ);
    while(1){
        c = rl_readchar();
        if (c == '\r')
            c = '\n';
        
        if (c == rl_termios->c_cc[VERASE]){ // Backspace or Delete
            if(rl_point > 0)
                rl_backspace();

            continue;
        }
        else if( c == rl_termios->c_cc[VKILL]){
            rl_clear();
            continue;
        }
        else if (c == EOF || c == rl_termios->c_cc[VEOF]) { // Control D
            rl_eof_found = 1;
            break;
        }
        else if (c == CTRL_P){
            navigate_history(previous_history);
            continue;
        }
        else if (c == CTRL_N){
            navigate_history(next_history);
            continue;
        }
        
        if (rl_end >= BUFFER_SIZ)
            break;
        
        rl_point++;
        rl_line_buffer[rl_end++] = c;
        fwrite(&c, 1, sizeof(char), rl_outstream);
        fflush(rl_outstream);

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
    if ((rl_eof_found && rl_end == 0) || ret)
        return NULL;
    return rl_line_buffer;
}