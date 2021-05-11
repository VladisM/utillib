/**
 * @defgroup array_group Arrays
 *
 * @brief Simple implementation of variable len arrays.
 *
 * In this module variable length arrays are implemented. This can be used for
 * example in strings implementations or anywhere where you want store variable
 * amount of relative small data pieces next to each other in memory. If you
 * don't care about memory layout please see list.h.
 *
 * To use variable length arrays you have to include header file for this
 * module and link it to your program. This module is contained in 'core'
 * package of utillib.
 *
 * In code you have to create pointer of array_t type pointing to NULL and pass
 * it to array_init(). This will create new array object with specified amount
 * of elements. You can then acces array itself by calling array_get_data() and
 * inspect size of array by array_get_size().
 *
 * In following example we will create new array of integers, double its size
 * and then set its content to zeroes.
 *
 * @code{.c}
 * #define DEFAULT_SIZE 64
 *
 * array_t *myArray = NULL;
 *
 * array_init(&myArray, sizeof(int), DEFAULT_SIZE);
 * array_enlarge(myArray);
 *
 * int *myData = array_get_data(myArray);
 * unsigned size = array_get_size(myArray);
 *
 * for(unsigned i = 0; i <= size; i++)
 *     myData[i] = 0;
 *
 * //don't forget to clean up
 * array_destroy(myArray);
 * @endcode
 *
 * @ingroup core_group
 *
 * @{
 */

#ifndef ARRAY_H_included
#define ARRAY_H_included

#include <stddef.h>

/**
 * @brief Structure to hold array object.
 */
typedef struct{
    void *payload;              /**< @brief Pointer to raw data. */
    size_t element_size;        /**< @brief Size of one element in array. */
    unsigned element_count;     /**< @brief Actual size of array. */
}array_t;

/**
 * @brief Initialize new empty array.
 *
 * @param ptr Pointer that will be set with new array.
 * @param element_size Size of one array item.
 * @param element_count Size of array in element count.
 *
 * @note Array will be malloced, don't forget to use array_destroy!
 */
extern void array_init(array_t **ptr, size_t element_size, unsigned element_count);

/**
 * @brief Deallocate memory used by array.
 *
 * @param ptr Pointer to array object.
 */
extern void array_destroy(array_t *ptr);

/**
 * @brief Double the size of array.
 *
 * @param ptr Pointer to array object.
 *
 * @note Will use realloc, data will be copied to new location too.
 */
extern void array_enlarge(array_t *ptr);

/**
 * @brief Get actual size of array.
 *
 * @param ptr Pointer to array object.
 *
 * @return Size of array (elements count).
 */
extern unsigned array_get_size(array_t *ptr);

/**
 * @brief Get pointer to raw data, to work with array object as regular array.
 *
 * @param ptr Pointer to array object.
 *
 * @warning Do not free returned pointer. You have to use array_destroy().
 *
 * @return Pointer to data array.
 */
extern void *array_get_data(array_t *ptr);

/**
 * @brief Get data at specified position of array.
 *
 * @param ptr Array object.
 * @param pos Position in array to look at.
 *
 * @return Pointer to specified data. It has to be dereferenced and set to
 * correct type.
 */
extern void *array_at(array_t *ptr, unsigned pos);

/**
 * @brief Set element of array to specified value.
 *
 * @param ptr Pointer to array object.
 * @param pos Position of element to be set.
 * @param val Value to set element to.
 */
extern void array_set(array_t *ptr, unsigned pos, void *val);

/**
 * @brief Set whole array to 0.
 *
 * @param ptr Pointer to array object.
 */
extern void array_cleanup(array_t *ptr);

#endif

/**
 * @}
 */
