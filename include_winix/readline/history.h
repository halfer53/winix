#ifndef __READLINE_HISTORY_H_
#define __READLINE_HISTORY_H_

typedef char *histdata_t;

typedef struct _hist_entry {
  char *line;
  char *data;
} HIST_ENTRY;

typedef struct _hist_state {
  HIST_ENTRY **entries;         /* Pointer to the entries themselves. */
  int offset;                   /* The location pointer within this array. */
  int length;                   /* Number of elements within this array. */
  int size;                     /* Number of slots allocated to this array. */
  int flags;
} HISTORY_STATE;


void add_history(char *line);
HIST_ENTRY * history_get (int offset);
HIST_ENTRY * previous_history ();
HIST_ENTRY * next_history ();
HISTORY_STATE * history_get_history_state ();

#endif
