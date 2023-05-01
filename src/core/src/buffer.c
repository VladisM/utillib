#include "buffer.h"

#include "array.h"
#include "dynmem.h"
#include "check.h"
#include "error.h"

#include <string.h>
#include <stdlib.h>

// -------------------------------------
// Implementation of core functionality

static inline buffer_t *_new_buffer(size_t element_size, unsigned element_count){
    buffer_t *tmp_buffer = (buffer_t *)malloc(sizeof(buffer_t));

    dynmem_check_malloc(tmp_buffer);

    memset(tmp_buffer, 0, sizeof(buffer_t));

    tmp_buffer->buffer_array = NULL;
    tmp_buffer->head = 0;
    tmp_buffer->tail = 0;

    array_init(&(tmp_buffer->buffer_array), element_size, element_count);

    return tmp_buffer;
}

static inline void _free_buffer(buffer_t *ptr){
    array_destroy(ptr->buffer_array);
    dynmem_free(ptr);
}

static inline unsigned _capacity(buffer_t *ptr){
    return array_get_size(ptr->buffer_array);
}

static inline unsigned _increment_pointer(buffer_t *ptr, unsigned x){
    return (x + 1) % _capacity(ptr);
}

static inline bool _full(buffer_t *ptr){
    return (_increment_pointer(ptr, ptr->head) == ptr->tail) ? true : false;
}

static inline bool _empty(buffer_t *ptr){
    return (ptr->head == ptr->tail) ? true : false;
}

static inline void _append(buffer_t *ptr, void *val){
    array_set(ptr->buffer_array, ptr->head, val);
}

static inline void _take(buffer_t *ptr, void *val){
    memcpy(val, array_at(ptr->buffer_array, ptr->tail), array_get_element_size(ptr->buffer_array));
}

static inline void _clear(buffer_t *ptr){
    ptr->head = 0;
    ptr->tail = 0;
}

static inline unsigned _count_empty(buffer_t *ptr){
    return (ptr->head >= ptr->tail) ? (_capacity(ptr) - (ptr->head - ptr->tail)) : (ptr->tail - ptr->head);
}

static inline unsigned _count_filed(buffer_t *ptr){
    return (ptr->head >= ptr->tail) ? (ptr->head - ptr->tail) : (_capacity(ptr) - (ptr->tail - ptr->head));
}

static inline void _move_head(buffer_t *ptr){
    ptr->head = _increment_pointer(ptr, ptr->head);
}

static inline void _move_tail(buffer_t *ptr){
    ptr->tail = _increment_pointer(ptr, ptr->tail);
}

// -------------------------------------
// Implementation of generic buffer

void buffer_init(buffer_t **ptr, size_t element_size, unsigned element_count){
    CHECK_NULL_ARGUMENT(ptr);
    CHECK_NOT_NULL_ARGUMENT(*ptr);

    if(element_count == 0)
        error("You can't create new buffer with zero elements!");

    if(element_size == 0)
        error("You can't create new buffer with elements size zero!");

    *ptr = _new_buffer(element_size, element_count + 1);
}

void buffer_destroy(buffer_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);
    _free_buffer(ptr);
}

bool buffer_store(buffer_t *ptr, void *val){
    CHECK_NULL_ARGUMENT(ptr);
    CHECK_NULL_ARGUMENT(val);

    if(_full(ptr))
        return false;

    _append(ptr, val);
    _move_head(ptr);

    return true;
}

bool buffer_take(buffer_t *ptr, void *val){
    CHECK_NULL_ARGUMENT(ptr);
    CHECK_NULL_ARGUMENT(val);

    if(_empty(ptr))
        return false;

    _take(ptr, val);
    _move_tail(ptr);

    return true;
}

bool buffer_peek(buffer_t *ptr, void *val){
    CHECK_NULL_ARGUMENT(ptr);
    CHECK_NULL_ARGUMENT(val);

    if(_empty(ptr))
        return false;

    _take(ptr, val);

    return true;
}

unsigned buffer_count_empty(buffer_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);
    return _count_empty(ptr) - 1;
}

unsigned buffer_count_filed(buffer_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);
    return _count_filed(ptr);
}

unsigned buffer_capacity(buffer_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);
    return _capacity(ptr) - 1;
}

bool buffer_full(buffer_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);
    return _full(ptr);
}

bool buffer_empty(buffer_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);
    return _empty(ptr);
}

void buffer_clear(buffer_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);
    _clear(ptr);
}

#ifndef NDEBUG

extern void *buffer_get_data(buffer_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);
    return array_get_data(ptr->buffer_array);
}

#endif
