#include "progress_bar.h"

#include <utillib/core.h>

#include <stdio.h>
#include <string.h>

static unsigned calc_percentage(progress_bar_t *this){
    return (unsigned)(((float)this->actual / (float)this->total) * 100);
}

static int print_percentage(progress_bar_t *this, unsigned percentage){
    return fprintf(this->stream, "%u%%", percentage);
}

static void cleanup_line(progress_bar_t *this){
    for(int i = 0; i < this->last_len; i++){
        fputc(' ', this->stream);
    }

    fputc('\r', this->stream);
    fflush(this->stream);
}

static void progress_bar_bar(progress_bar_t *this){
    unsigned progress = calc_percentage(this);
    int chars_printed = strlen(this->title);

    if(progress > 100){
        error("Progress argument out of range.");
    }

    cleanup_line(this);

    fputs(this->title, this->stream);
    fputs(" [", this->stream);

    for(unsigned i = 0; i < 50; i++){
        fputc(((i * 2) < progress) ? '#' : ' ', this->stream);
    }

    fputs("] ", this->stream);
    chars_printed += print_percentage(this, progress);
    fputc('\r', this->stream);
    fflush(this->stream);

    chars_printed += 7;
    chars_printed += 50;
    this->last_len = chars_printed;
}

static void progress_bar_text(progress_bar_t *this){
    unsigned progress = calc_percentage(this);
    int chars_printed = strlen(this->title);

    if(progress > 100){
        error("Progress argument out of range.");
    }

    cleanup_line(this);

    fputs(this->title, this->stream);
    fputc(' ', this->stream);
    chars_printed += print_percentage(this, progress);
    fputc('\r', this->stream);
    fflush(this->stream);

    chars_printed += 1;
    this->last_len = chars_printed;
}

void progress_bar_init(progress_bar_t **this, char *title, unsigned total, FILE *stream, progress_bar_type_t type){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(title);
    CHECK_NULL_ARGUMENT(stream);
    CHECK_NOT_NULL_ARGUMENT(*this);

    progress_bar_t *tmp = (progress_bar_t *)dynmem_malloc(sizeof(progress_bar_t));

    tmp->actual = 0;
    tmp->total = total;
    tmp->last_len = 0;
    tmp->stream = stream;
    tmp->type = type;

    tmp->title = dynmem_strdup(title);

    *this = tmp;
}

void progress_bar_destroy(progress_bar_t *this){
    CHECK_NULL_ARGUMENT(this);

    dynmem_free(this->title);
    dynmem_free(this);
}

void progress_bar_update(progress_bar_t *this, unsigned actual){
    CHECK_NULL_ARGUMENT(this);

    this->actual = actual;

    switch(this->type){
        case PROGRESS_BAR_BAR:
            progress_bar_bar(this);
            break;
        case PROGRESS_BAR_TEXT:
            progress_bar_text(this);
            break;
    }
}

void progress_bar_clean(progress_bar_t *this){
    CHECK_NULL_ARGUMENT(this);
    fputs("\r\n", this->stream);
    fflush(this->stream);
}
