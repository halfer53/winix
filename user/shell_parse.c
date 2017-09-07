/**
 * 
 * Shell parser
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:13:40
 * 
*/
#include "shell.h"

int isseparator(char c){
    return isspace(c); 
}

int parse(char *line, struct cmdLine *sc, struct cmd *builtin_commands){
    int argc = 0;
    char *in;
    struct cmd *handler;
    sc->argv[0] = '\0';
    strcpy(sc->buf,line); 
    in = sc->buf;
    sc->cmdStart[0] = 0;    /* the 0th command starts at argv[0] */
    sc->numCommands = 0;
    
    while(argc < MAX_ARGS-1 && sc->numCommands < MAX_COMMANDS){
        
        while(isspace(*in)) /* skip initial whitespace */
            ++in;
        if(*in == '\0')   /* end-of-line, we're done */
            break;

        switch(*in){
            //place holder for special characters e.g. redirections: >, >>, <
            //case '>'

            default:    /* it's just a regular argument word */
                sc->argv[argc++] = in;
                break;
        }
        
        while(*in && !isseparator(*in))
            in++;
        if(*in == '\0') //if its the end of the input string, then we are done
            break;
        else
            *in++ = '\0'; //terminate the current argument and go to the next one
    }
    
    sc->argv[argc] = NULL;  // terminate the last command 
    sc->argv[argc + 1] = NULL;  // marker meaning "no more commands" 
    sc->argc = argc;
    
    if(argc > 0)
        ++sc->numCommands;
    
    if(argc == 0){
        sc->argv[0] = in;
        (sc->argv)[0][0] = '\0';
    }
       
    return 0;
}
