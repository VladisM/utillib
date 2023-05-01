#ifndef STRING_H_included
#define STRING_H_included

#include <stdbool.h>
#include <stdarg.h>

#include "array.h"

typedef array_t string_t;

extern void string_init(string_t **s);
extern void string_init_1(string_t **s, char *data);
extern void string_destroy(string_t *s);

extern char *string_get(string_t *s);
extern char string_at(string_t *s, unsigned pos);

extern string_t *string_duplicate(string_t *s);
extern bool string_bcompare(string_t *s1, string_t *s2);
extern int string_compare(string_t *s1, string_t *s2);
extern unsigned string_length(string_t *s);
extern void string_concatenate(string_t *s1, string_t *s2);

extern void string_puts(string_t *s, char *data);
extern void string_printf(string_t *s, char *format, ...);
extern void string_vprintf(string_t *s, char *format, va_list args);

extern void string_append(string_t *s, char *data);
extern void string_appendf(string_t *s, char *format, ...);
extern void string_vappendf(string_t *s, char *format, va_list args);

#endif
