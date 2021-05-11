#include "array.h"

#include "dynmem.h"
#include "error.h"
#include "check.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static array_t *_new_array(size_t element_size, unsigned element_count);
static void _free_array(array_t *array);
static void _double_size(array_t *array);

// -------------------------------------
// Implemenation of core functionality

static array_t *_new_array(size_t element_size, unsigned element_count){
    array_t *tmp_array = NULL;
    void *tmp_data = NULL;

    tmp_array = (array_t *)malloc(sizeof(array_t));
    tmp_data = malloc(element_count * element_size);

    dynmem_check_malloc(tmp_array);
    dynmem_check_malloc(tmp_data);

    tmp_array->element_count = element_count;
    tmp_array->element_size = element_size;
    tmp_array->payload = tmp_data;

    return tmp_array;
}

static void _free_array(array_t *array){
    free(array->payload);
    free(array);
}

static void _double_size(array_t *array){
    void *tmp_data = NULL;

    size_t new_count = array->element_count * 2;
    size_t new_size = new_count * array->element_size;

    tmp_data = realloc(array->payload, new_size);
    dynmem_check_malloc(tmp_data);

    array->payload = tmp_data;
    array->element_count = new_count;
}

static void *_at(array_t *array, unsigned position){
    char *tmp = (char *)array->payload;

    size_t offset = array->element_size / sizeof(char);
    tmp += (offset * position);

    return (void *)tmp;
}

static void _set(array_t *array, unsigned position, void *value){
    void *element = _at(array, position);
    memcpy(element, value, array->element_size);
}

static void _clear_data(array_t *array){
    memset(array->payload, 0, array->element_count);
}

// -------------------------------------
// Implemenation of generic arrays

void array_init(array_t **ptr, size_t element_size, unsigned element_count){
    CHECK_NULL_ARGUMENT(ptr);
    CHECK_NOT_NULL_ARGUMENT(*ptr);

    if(element_count == 0)
        error("You can't create new array with zero elements!");

    *ptr = _new_array(element_size, element_count);
}

void array_destroy(array_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);

    _free_array(ptr);
}

void array_enlarge(array_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);

    if(ptr->payload == NULL)
        error("Array isn't initialized!");

    _double_size(ptr);
}

unsigned array_get_size(array_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);

    return ptr->element_count;
}

void *array_get_data(array_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);

    return _at(ptr, 0);
}

void *array_at(array_t *ptr, unsigned pos){
    CHECK_NULL_ARGUMENT(ptr);

    if(pos >= ptr->element_count)
        error("Index out of array!");

    return _at(ptr, pos);
}

void array_set(array_t *ptr, unsigned pos, void *val){
    CHECK_NULL_ARGUMENT(ptr);
    CHECK_NULL_ARGUMENT(val);

    if(pos >= ptr->element_count)
        error("Index out of array!");

    _set(ptr, pos, val);
}

void array_cleanup(array_t *ptr){
    CHECK_NULL_ARGUMENT(ptr);

    _clear_data(ptr);
}
