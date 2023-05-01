#include "atexit.h"

#include "error.h"
#include "list.h"
#include "check.h"

#include <stdlib.h>
#include <stdbool.h>

typedef void atexit_signature(void);

static stack_t *user_calls = NULL;
static bool initialized = false;

void atexit_utillib_core_atexit(void){
    if(initialized == false)
        error("Called atexit_utillib_core_atexit but atexit isn't initialized!");

    while(stack_count(user_calls) > 0){
        void (*tmp)(void) = NULL;
        stack_pop(user_calls, &tmp);
        (*tmp)();
    }

    stack_destroy(user_calls);
}

void atexit_register(void (*f)(void)){
    CHECK_NULL_ARGUMENT(f);

    if(initialized == false)
        atexit_init();

    stack_push(user_calls, (void *)&f);
}

void atexit_init(void){
    if(initialized)
        return;

    stack_init(&user_calls, sizeof(atexit_signature *));

    if(atexit(atexit_utillib_core_atexit))
        error("Failed to register atexit function!");

    initialized = true;
}
