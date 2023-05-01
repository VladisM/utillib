/**
 * @defgroup error_group Error
 *
 * @brief Error handling.
 *
 * @todo More documentation.
 *
 * @ingroup core_group
 *
 * @{
 */

#ifndef ERROR_H_included
#define ERROR_H_included

/**
 * @brief Raise error and exit program.
 *
 * @param err_msg Message that will be shown at program exit.
 */
extern void  __attribute__((noreturn)) error(char *err_msg);

#endif

/**
 * @}
 */
