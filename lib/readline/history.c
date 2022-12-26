#include <readline/history.h>
#include <stdlib.h>
#include <string.h>

static HISTORY_STATE history_state;
#define INITIAL_HISTORY_SIZE 10

HISTORY_STATE* history_get_history_state (){
    return &history_state;
}

void init_history(){
    if (history_state.entries != NULL)
        return;
    history_state.entries = malloc(sizeof(HIST_ENTRY*) * INITIAL_HISTORY_SIZE);
    history_state.offset = 0;
    history_state.length = 0;
    history_state.size = INITIAL_HISTORY_SIZE;
    history_state.flags = 0;
}

void add_history(char *line){
    HIST_ENTRY* entry = malloc(sizeof(HIST_ENTRY));
    init_history();
    entry->line = strdup(line);
    entry->data = NULL;
    if (history_state.length >= history_state.size){
        history_state.size *= 2;
        history_state.entries = realloc(history_state.entries, sizeof(HIST_ENTRY*) * history_state.size);
    }
    history_state.entries[history_state.length] = entry;
    history_state.offset++;
    history_state.length++;
}

HIST_ENTRY * history_get (int offset){
    if (offset < 0 || offset >= history_state.length)
        return NULL;
    return history_state.entries[offset];
}

HIST_ENTRY *previous_history (){
    if (history_state.offset <= 0)
        return NULL;
    history_state.offset--;
    return history_state.entries[history_state.offset];
}

HIST_ENTRY * next_history (){
    if (history_state.offset >= history_state.length - 1)
        return NULL;
    history_state.offset++;
    return history_state.entries[history_state.offset];
}

HIST_ENTRY* current_history (){
    if (history_state.offset < 0 || history_state.offset >= history_state.length)
        return NULL;
    
    return history_state.entries[history_state.offset];
}
