#ifndef __READLINE_READLINE_H_
#define __READLINE_READLINE_H_

#include <stdio.h>
#include <termios.h>

extern char * rl_line_buffer; // This is the line gathered so far. You are welcome to modify the contents of the line, but see Allowing Undoing. The function rl_extend_line_buffer is available to increase the memory allocated to rl_line_buffer.
extern int rl_point; // The offset of the current cursor position in rl_line_buffer (the point). This is always between 0 and rl_end.
extern int rl_end; // The number of characters present in rl_line_buffer. When rl_point is at the end of the line, rl_point and rl_end are equal.
extern char* rl_prompt; // The prompt Readline uses. This is set from the argument to readline(), and should not be assigned to directly. The rl_set_prompt() function (see Redisplay) may be used to modify the prompt string after calling readline().
extern int rl_done; // This is a flag that is set to 1 when readline() has finished reading a line. It is useful for applications that want to call readline() multiple times in a loop, and do something else in between calls. The application should set this to 0 before calling readline() again.
extern int rl_eof_found; // This is a flag that is set to 1 when readline() has found EOF on the input stream. It is useful for applications that want to call readline() multiple times in a loop, and do something else in between calls. The application should set this to 0 before calling readline() again.

extern struct termios rl_ttyset; // The terminal settings that Readline uses. This is set from the argument to readline(), and should not be assigned to directly. The rl_set_tty() function (see Redisplay) may be used to modify the terminal settings after calling readline().

extern FILE * rl_instream; // The stdio stream from which Readline reads input.
extern FILE * rl_outstream; // The stdio stream to which Readline performs output.

char *readline(char *);

#endif
