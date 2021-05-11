#include "string.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "array.h"
#include "check.h"
#include "error.h"
#include "dynmem.h"

#ifndef DEFAULT_STRING_SIZE
#define DEFAULT_STRING_SIZE 64
#endif

void string_init(string_t **s){
    array_init(s, sizeof(char), DEFAULT_STRING_SIZE);
    array_cleanup(*s);
}

void string_init_1(string_t **s, char *data){
    CHECK_NULL_ARGUMENT(data);
    string_init(s);
    string_puts(*s, data);
}

void string_destroy(string_t *s){
    array_destroy(s);
}

char *string_get(string_t *s){
    return array_get_data(s);
}

char string_at(string_t *s, unsigned pos){
    CHECK_NULL_ARGUMENT(s);
    return *(char *)(array_at(s, pos));
}

string_t *string_duplicate(string_t *s){
    string_t *tmp = NULL;
    array_init(&tmp, sizeof(char), array_get_size(s));

    for(unsigned i = 0; i < array_get_size(s); i++){
        array_set(tmp, i, array_at(s, i));
    }

    return tmp;
}

bool string_bcompare(string_t *s1, string_t *s2){
    if(string_compare(s1, s2) == 0)
        return true;
    else
        return false;
}

int string_compare(string_t *s1, string_t *s2){
    CHECK_NULL_ARGUMENT(s1);
    CHECK_NULL_ARGUMENT(s2);

    return strcmp((char *)array_get_data(s1), (char *)array_get_data(s2));
}

unsigned string_length(string_t *s){
    CHECK_NULL_ARGUMENT(s);

    return (unsigned)strlen((char *)array_get_data(s));
}

void string_concatenate(string_t *s1, string_t *s2){
    CHECK_NULL_ARGUMENT(s1);
    CHECK_NULL_ARGUMENT(s2);

    char *s1_data = array_get_data(s1);
    char *s2_data = array_get_data(s2);

    long size_needed = strlen(s1_data) + strlen(s2_data) + 1;

    while(size_needed > array_get_size(s1)){
        array_enlarge(s1);
    }

    s1_data = array_get_data(s1);
    s2_data = array_get_data(s2);

    char *ret = strcat(s1_data, s2_data);

    if(ret != s1_data){
        error("Something went horribly wrong in library strcat!");
    }
}

void string_puts(string_t *s, char *data){
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(data);

    if(strlen(data) == 0){
        return;
    }

    long size_needed = strlen(data) + 1;

    while(size_needed > array_get_size(s)){
        array_enlarge(s);
    }

    array_cleanup(s);

    char *tmp = array_get_data(s);
    sprintf(tmp, "%s", data);
}

void string_printf(string_t *s, char *format, ...){
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(format);

    if(strlen(format) == 0){
        return;
    }

    va_list args;
    va_start(args, format);
    string_vprintf(s, format, args);
    va_end(args);
}

void string_vprintf(string_t *s, char *format, va_list args){
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(format);

    if(strlen(format) == 0){
        return;
    }

    va_list args_1;
    va_list args_2;

    va_copy(args_1, args);
    va_copy(args_2, args);

    int length = vsnprintf(NULL, 0, format, args_1);
    char *tmp = dynmem_calloc(length + 1, sizeof(char));
    vsprintf(tmp, format, args_2);
    string_puts(s, tmp);

    dynmem_free(tmp);

    va_end(args_1);
    va_end(args_2);
}

void string_append(string_t *s, char *data){
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(data);

    string_t *tmp = NULL;

    string_init_1(&tmp, data);
    string_concatenate(s, tmp);
    string_destroy(tmp);
}

void string_appendf(string_t *s, char *format, ...){
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(format);

    if(strlen(format) == 0){
        return;
    }

    va_list args;
    va_start(args, format);
    string_vappendf(s, format, args);
    va_end(args);
}

void string_vappendf(string_t *s, char *format, va_list args){
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(format);

    if(strlen(format) == 0){
        return;
    }

    string_t *tmp = NULL;

    string_init(&tmp);
    string_vprintf(tmp, format, args);
    string_concatenate(s, tmp);
    string_destroy(tmp);
}
