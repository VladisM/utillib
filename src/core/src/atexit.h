/**
 * @defgroup atexit_group Atexit
 *
 * @brief Simple wrapper around atexit function.
 *
 * Library atexit have some disadvanteges. Firstly, it is limited to 32 or more
 * (according to ISO C) functions calls. This mean, in large scale application
 * you may run out of space for it. This simple wrapper is using dynamically
 * allocated memory to store unlimited count of calls.
 *
 * Also it is pretty neat solution that ensure that all deinitialization of
 * utillib itself is done in right order and it isn't mixed with application
 * exit handlers.
 *
 * Ussage is pretty straightforward, simply call atexit_init() at the
 * begininning of your application and then use atexit_register().
 *
 * @ingroup core_group
 *
 * @{
 */

#ifndef ATEXIT_H_included
#define ATEXIT_H_included

/**
 * @brief Initialize module.
 */
extern void atexit_init(void);

/**
 * @brief Register exit callback function.
 *
 * Registered function will get called in reverse order when program ends using
 * exit() function or returning from main().
 *
 * @param f Pointer to function that will be called.
 */
extern void atexit_register(void (*f)(void));

#endif

/**
 * @}
 */
