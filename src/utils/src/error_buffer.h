/**
 * @defgroup error_buffer_group Error buffer
 *
 * @brief HHold string useful for error mesage.
 *
 * @todo missing documentation
 * @todo missing test
 *
 * @ingroup utils_group
 *
 * @{
 */

#ifndef ERROR_BUFFER_H_included
#define ERROR_BUFFER_H_included

#include <utillib/core.h>

typedef string_t error_t;

/**
 * @brief Initialize string buffer.
 *
 * @param error_buffer Pointer to buffer to work with.
 */
void error_buffer_init(error_t **error_buffer);

/**
 * @brief Destroy and free memory used by buffer.
 *
 * @param error_buffer Pointer to buffer to work with.
 */
void error_buffer_destroy(error_t *error_buffer);

/**
 * @brief Get error message currently set.
 *
 * @param error_buffer Pointer to buffer to work with.
 * @return char* Error message.
 */
char * error_buffer_get(error_t *error_buffer);

/**
 * @brief Append into error message. Each call will be separated with '\r\n'
 * sequence.
 *
 * @param error_buffer Pointer to buffer to work with.
 * @param fmt Printf-like format.
 * @param ... Printf-like arguments.
 */
void error_buffer_write(error_t *error_buffer, char *fmt, ...);

#endif

/**
 * @}
 */
