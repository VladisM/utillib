#include "error_buffer.h"

#include <utillib/core.h>

void error_buffer_init(error_buffer_t **error_buffer){
    string_init((string_t **)error_buffer);
}

void error_buffer_destroy(error_buffer_t *error_buffer){
    string_destroy((string_t *)error_buffer);
}

char * error_buffer_get(error_buffer_t *error_buffer){
    CHECK_NULL_ARGUMENT(error_buffer);
    return string_get(error_buffer);
}

void error_buffer_write(error_buffer_t *error_buffer, char *fmt, ...){
    CHECK_NULL_ARGUMENT(fmt);
    CHECK_NULL_ARGUMENT(error_buffer);

    va_list args;
    va_start(args, fmt);
    string_vappendf(error_buffer, fmt, args);
    va_end(args);

    string_appendf(error_buffer, "%s", "\r\n");
}
