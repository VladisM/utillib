#ifndef PROGRESS_BAR_H_included
#define PROGRESS_BAR_H_included

#include <stdio.h>

typedef enum {
    PROGRESS_BAR_BAR = 0,
    PROGRESS_BAR_TEXT
} progress_bar_type_t;

typedef struct {
    unsigned total;
    unsigned actual;
    char *title;
    FILE * stream;
    int last_len;
    progress_bar_type_t type;
} progress_bar_t;

extern void progress_bar_init(progress_bar_t **this, char *title, unsigned total, FILE *stream, progress_bar_type_t type);
extern void progress_bar_destroy(progress_bar_t *this);

extern void progress_bar_update(progress_bar_t *this, unsigned actual);
extern void progress_bar_clean(progress_bar_t *this);

#endif
