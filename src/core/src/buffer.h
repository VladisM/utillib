/**
 * @defgroup buffer_group Buffer
 *
 * @brief Implementation of circular buffer.
 *
 * @note Internally buffer are implemented using arrays.
 *
 * @note Size of internal array is incremented by 1 to be able
 * to recognize between empty and full state.
 *
 * This is simple implementation of circular buffer that will
 * allow you store data in FIFO manner with fixed memory usage
 * and without any relocation of data. It is useful if you want
 * for example store and process data from bus.
 *
 * @code{.c}
 * #define BUFFER_SIZE 64
 *
 * buffer_t *buffer = NULL;
 * buffer_init(&buffer, sizeof(uint8_t), BUFFER_SIZE);
 *
 * uint8_t data_to_store = 0;
 * buffer_store(buffer, (void *)&data_to_store);
 *
 * uint8_t data_read = 0;
 * buffer_take(buffer, (void *)&data_read);
 *
 * buffer_destroy(buffer);
 * @endcode
 *
 * @ingroup core_group
 *
 * @{
 */

#ifndef BUFFER_H_included
#define BUFFER_H_included

#include <stdbool.h>

#include "array.h"

/**
 * @brief Structure to hold buffer object.
 */
typedef struct{
    array_t *buffer_array;  /**< @brief Underlying array. */
    unsigned tail;          /**< @brief Pointer to tail of buffer. Last slot with data. */
    unsigned head;          /**< @brief Pointer to head of buffer. Next empty slot.*/
} buffer_t;

/**
 * @brief Initialize buffer object.
 *
 * @param ptr Pointer to pointer where address of new buffer will be stored.
 * @param element_size Size of one element in buffer (e.g. sizeof(uint8_t))
 * @param element_count Count of element you want to store.
 */
extern void buffer_init(buffer_t **ptr, size_t element_size, unsigned element_count);

/**
 * @brief Destroy previously initialized buffer.
 *
 * @param ptr Pointer to buffer.
 */
extern void buffer_destroy(buffer_t *ptr);

/**
 * @brief Insert item into buffer.
 *
 * @param ptr Pointer to buffer object.
 * @param val Value to be stored.
 * @return true Value was stored.
 * @return false Value wasn't stored.
 */
extern bool buffer_store(buffer_t *ptr, void *val);

/**
 * @brief Take out item from buffer.
 *
 * @param ptr Pointer to buffer object.
 * @param val Pointer where value from buffer will be copied to.
 * @return true Data successfully taken from buffer.
 * @return false Reading buffer failed.
 */
extern bool buffer_take(buffer_t *ptr, void *val);

/**
 * @brief Reading out item from buffer without removing it from buffer.
 *
 * @param ptr Pointer to buffer object.
 * @param val Pointer where value from buffer will be copied to.
 * @return true Data successfully taken from buffer.
 * @return false Reading buffer failed.
 */
extern bool buffer_peek(buffer_t *ptr, void *val);

/**
 * @brief Count how many items are in buffer.
 *
 * @param ptr Pointer to buffer object.
 * @return unsigned Count of items in buffer.
 */
extern unsigned buffer_count_filed(buffer_t *ptr);

/**
 * @brief Count how many empty slots are in buffer.
 *
 * @param ptr Pointer to buffer object.
 * @return unsigned Count of free slots in buffer.
 */
extern unsigned buffer_count_empty(buffer_t *ptr);

/**
 * @brief Get total capacity of buffer.
 *
 * @param ptr Pointer to buffer object.
 * @return unsigned Total count of slots in buffer.
 */
extern unsigned buffer_capacity(buffer_t *ptr);

/**
 * @brief Check if buffer is full.
 *
 * @note Buffer is marked as full once all slots are depleted.
 *
 * @param ptr Pointer to buffer object.
 * @return true Buffer is full.
 * @return false Buffer isn't full.
 */
extern bool buffer_full(buffer_t *ptr);

/**
 * @brief Check if buffer is empty.
 *
 * @note Buffer is marked as non empty once there is at least one stored item.
 *
 * @param ptr Pointer to buffer object.
 * @return true Buffer is empty.
 * @return false Buffer isn't empty.
 */
extern bool buffer_empty(buffer_t *ptr);

/**
 * @brief Remove all data from buffer.
 *
 * @note Implemented only as manipulation with head and tail pointers.
 *
 * @param ptr Pointer to buffer object.
 */
extern void buffer_clear(buffer_t *ptr);

#ifndef NDEBUG

/**
 * @brief Get raw data from internal array.
 * @warning This function is only for testing purposes.
 * @param ptr Pointer to buffer object.
 * @return void* Pointer to payload of underlying array.
 */
extern void *buffer_get_data(buffer_t *ptr);

#endif

#endif

/**
 * @}
 */
