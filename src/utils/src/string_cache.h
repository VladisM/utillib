/**
 * @defgroup string_cache_group String Cache
 *
 * @brief Cache strings to save memory.
 *
 * @ingroup utils_group
 *
 * @{
 */

#ifndef STRING_CACHE_H_included
#define STRING_CACHE_H_included

#include <utillib/core.h>

typedef list_t string_cache_t;

/**
 * @brief Create new cache object.
 *
 * @param cache Pointer to cache object.
 */
void string_cache_new(string_cache_t **cache);

/**
 * @brief Destroy cache object.
 *
 * @param cache Pointer to cache object.
 */
void string_cache_destroy(string_cache_t *cache);

/**
 * @brief Process string.
 *
 * @param cache Pointer to cache object.
 * @param string Pointer to string to process.
 *
 * @return Pointer to string.
 */
char *string_cache_process(string_cache_t *cache, char *string);

#endif

/**
 * @}
 */