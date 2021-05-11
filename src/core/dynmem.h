/**
 * @defgroup dynmem_group Dynmem
 *
 * @brief Dynamic memory allocation with something like "garbage collector".
 *
 * @todo missing documentation
 *
 * @ingroup core_group
 *
 * @{
 */

#ifndef DYNMEM_H_included
#define DYNMEM_H_included

#include <stddef.h>

extern void dynmem_check_malloc(void *p);

extern void *dynmem_malloc(size_t size);
extern void *dynmem_calloc(size_t num, size_t size);
extern void *dynmem_realloc(void *ptr, size_t new_size);

extern void dynmem_free(void *p);

extern char *dynmem_strdup(char *s);
#endif

/**
 * @}
 */
