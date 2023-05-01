#ifndef CHECK_H_included
#define CHECK_H_included

/**
 * @brief Check if argument is NULL and if so it throw an error.
 */
#define CHECK_NULL_ARGUMENT(x) check_null_argument((void*)(x), (__func__))

/**
 * @brief Check if argument is not NULL and if so it throw and error.
 */
#define CHECK_NOT_NULL_ARGUMENT(x) check_not_null_argument((void*)(x), (__func__))

/**
 * @brief Function that implement NULL check.
 * @note Please call macro CHECK_NULL_ARGUMENT instead.
 * @param argument Pointer to be checked.
 * @param function_name Function name where this is called.
 */
extern void check_null_argument(void *argument, const char function_name[]);

/**
 * @brief Function that implement not NULL check.
 * @note Please call macro CHECK_NULL_ARGUMENT instead.
 * @param argument Pointer to be checked.
 * @param function_name Function name where this is called.
 */
extern void check_not_null_argument(void *argument, const char function_name[]);

#endif
