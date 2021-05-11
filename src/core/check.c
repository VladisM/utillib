#include "check.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "error.h"

static char error_buffer[256];

void check_null_argument(void *argument, const char function_name[]){
    if(argument == NULL){
        memset((void *)error_buffer, '\0', sizeof error_buffer);
        snprintf(error_buffer, sizeof error_buffer, "Null argument passed into function %s!", function_name);
        error(error_buffer);
    }
}

void check_not_null_argument(void *argument, const char function_name[]){
    if(argument != NULL){
        memset((void *)error_buffer, '\0', sizeof error_buffer);
        snprintf(error_buffer, sizeof error_buffer, "Argument passed into %s should be NULL but it isn't!", function_name);
        error(error_buffer);
    }
}
