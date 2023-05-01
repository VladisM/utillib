#include "dynmem.h"

#include "list.h"
#include "error.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void dynmem_check_malloc(void *p){
    if(p == NULL)
        error("Check malloc failed!");
}

void *dynmem_malloc(size_t size){
    void *tmp = malloc(size);
    dynmem_check_malloc(tmp);
    return tmp;
}

void *dynmem_calloc(size_t num, size_t size){
    void *tmp = calloc(num, size);
    dynmem_check_malloc(tmp);
    return tmp;
}

void *dynmem_realloc(void *ptr, size_t new_size){
    if(ptr == NULL)
        return NULL;

    void *tmp = realloc(ptr, new_size);
    dynmem_check_malloc(tmp);

    return tmp;
}

void dynmem_free(void *p){
    if(p == NULL)
        return;

    free(p);
}

char *dynmem_strdup(char *s){
    if(s == NULL){
        return NULL;
    }

    unsigned long len = strlen(s);
    char *tmp = (char *)dynmem_calloc(len + 1, sizeof(char));
    memcpy((void *)tmp, (void *)s, len + 1);

    return tmp;
}
