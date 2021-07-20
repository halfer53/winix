#include <stdlib.h>
#include "parse.h"

static int isShellSymb(char c) {
    return (c)=='|' || (c)=='<' || (c)=='>' || (c)== '\'' || (c) == '=' || (c) == '"' || (c) == '`';
}


static int isSeparator(char c) {
    return isspace(c) || isShellSymb(c);
}

/*
** Parse: parse a simple nsh command line, and put the results into sc.
** INPUT: char *line, a string containing whitespace-separated tokens.
** OUTPUT: struct commandLine *sc
** NOTE: No syntax checking occurs.  Empty commands are not detected,
**      for example, so the calling function needs to do syntax checking.
** RETURN VALUES: 0 on success.  (Currently it always returns 0.)
*/
int parse(char *input_line, struct cmdLine *sc)
{
    int argc = 0;
    char* line;

    strlcpy(sc->buf, input_line, MAX_LINE);
    line = sc->buf;

    sc->infile = sc->outfile = sc->argv[0] = NULL;
    sc->append = sc->numCommands = sc->background = 0;
    sc->cmdStart[0] = 0;    /* the 0th command starts at argv[0] */
    sc->env = sc->env_val =  NULL;

    
    /*
    ** This is the main loop, reading arguments into argv[].
    ** We check against MAX_ARGS-1 since the last one must be NULL.
    */
    while(argc < MAX_ARGS-1 && sc->numCommands < MAX_COMMANDS)
    {
        /*
        ** At this point, "line" is pointing just beyond the end of the
        ** previous argument (or the very first character if this is
        ** the first time through the loop), which should be a whitespace
        ** character.
        */

        while(isspace(*line)) /* skip initial whitespace */
            ++line;
        if(*line == '\0')   /* end-of-line, we're done */
            break;

        /*
        ** At this point, "line" is pointing at a non-space character; it
        ** may be the start of an argument, or it may be a special character.
        */
        
        switch(*line)
        {
        case '|':
            /* terminate the previous argument and bump to next character */
            *line++ = '\0';
            sc->argv[argc++] = NULL;    /* terminate the command */
            /* the next command will start at this position */
            sc->cmdStart[++sc->numCommands] = argc;
            continue;   /* go back to the top of the while loop */

        case '<':
            *line++ = '\0'; /* terminate argument and go to next character */
            while(isspace(*line))   /* skip whitespace */
                ++line;
            sc->infile = line;
            break;

        case '>':
            *line++ = '\0'; /* terminate the argument */
            if(*line == '>') /* second '>' means append */
            {
                sc->append = 1;
                line++;
            }
            while(isspace(*line))   /* skip whitespace */
                ++line;
            sc->outfile = line;
            break;
        case '&':
            sc->background = 1;
            break;
        case '\\':
            sc->argv[argc++] = ++line;
            line++;
            break;
        case '\'':
            sc->argv[argc++] = ++line;
            while(*line != '\''){line++;}
            *line = '\0';
            break;
        case '"':
            sc->argv[argc++] = line++;
            while(*line != '"'){line++;}
            *++line = '\0';
                line++;
                continue;
        case '`':
            sc->argv[argc++] = line++;
            while(*line != '`'){line++;}
            *++line = '\0';
                line++;
                continue;
        /*
        ** Here would be a good place to check for other special characters
        ** (quotes, '&', etc) and do something clever.
        */

        default:    /* it's just a regular argument word */
            sc->argv[argc++] = line;
            break;
        }
        
        while(*line && !isSeparator(*line))  /* find the end of the argument */
            ++line;
        if(*line == '='){
            int symbol;
            *line++ = '\0';
            
            sc->env = sc->argv[argc-1];     /* env points to the previous word */   
            sc->argv[argc++] = line;
            sc->env_val = sc->argv[argc -1]; 

            // skip strings inside double quotes or single quotes
            if(*line == '"')
                symbol = *line;
            if(*line == '\'')
                symbol = *line;
            line++;
            while(*line && *line != symbol)
                line++;            
            line++;
        }
        if(*line == '\0')        /* end-of-line, we're done */
            break;
        else
            *line++ = '\0'; /* terminate the argument and go to next char */
    }

    /* We are at the very end of "line" */
    sc->argv[argc] = NULL;  /* terminate the last command */
    sc->argv[argc + 1] = NULL;  /* marker meaning "no more commands" */

    /*
    ** There's a nasty special case to handle here.  If the command line
    ** contains any commands, then numCommands can simply be set to
    ** (number of '|' symbols + 1).  Note that the only place above
    ** that numCommands is increased is in the "case '|'" of the switch.
    ** So we should add 1 here.  BUT, if there were NO commands (ie, the
    ** command line was empty), then we should NOT add 1.
    */
    if(argc > 0){
        ++sc->numCommands;
    }

    if(argc == 0){
        sc->argv[0] = sc->buf;
        (sc->argv)[0][0] = '\0';
    }
    sc->argc = argc;
    return 0;
}

#define DOUBLE_QUOTES   1
#define SINGLE_QUOTE    2

int parse_quotes(char *input, char* buffer, int buf_len){
    char* in = input;
    char* out = buffer;
    int mode = 0;

    if(*in == '"'){
        in++;
        mode = DOUBLE_QUOTES;
    }else if(*in == '\''){
        in++;
        mode = SINGLE_QUOTE;
    }
        
    while(*in){

        if(*in == '"' || *in == '\'')
            *in = '\0';

        if(*in == '$'){ // if env
            int bak_char;
            char *envval, *val_start = ++in;// one for '$'
            while(*in && (isupper(*in) || islower(*in))){
                in++;
            }
            bak_char = *in;
            *in = '\0';
            
            // printf("search %s\n", val_start);
            
            // insert the environment value at the $
            envval = getenv(val_start);
            if(envval == NULL){
                *in = bak_char;
                continue;
            }

            // concatenate the environment value to the output 
            // buffer
            *out = '\0';
            strlcat(out, envval, buf_len);
            out += strlen(envval);
            *in = bak_char;
        }else{
            *out++ = *in++;
        }
    }
    *out = '\0';
    return 0;
}

