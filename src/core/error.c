#include "error.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef ENABLE_CALL_STACK_DUMP_AT_ERROR

    #include <execinfo.h>

    void dump_call_stack(void){
        int n = 0;
        void *buffer[100];
        char **strings;

        fprintf(stderr, "\r\nCall stack:\r\n");

        n = backtrace(buffer, sizeof buffer);
        strings = backtrace_symbols(buffer, n);

        if(strings == NULL){
            fprintf(stderr, " Failed to get call stack!\r\n");
            return;
        }

        for(int i = 0; i < n; i++){
            fprintf(stderr, " %s\r\n", strings[i]);
        }

        fprintf(stderr, "\r\nUse -rdynamic to get more verbose call stack.\r\n");
    }

#else

    void dump_call_stack(void){
        return;
    }

#endif

void error(char *err_msg){
    if(err_msg != NULL)
        fprintf(stderr, "Error! %s\r\n", err_msg);

    dump_call_stack();

    fflush(stderr);
    exit(EXIT_FAILURE);
    while(1);
}
