#include "common.h"

#include <utillib/core.h>

#include <stdio.h>
#include <stdlib.h>

bool utillib_file_write_file(string_t *data, char *filename){
    CHECK_NULL_ARGUMENT(filename);
    CHECK_NULL_ARGUMENT(data);

    FILE *fp = fopen(filename, "wb");

    if(fp == NULL){
        return false;
    }

    for(unsigned i = 0; string_at(data, i) != '\0'; i++){
        fprintf(fp, "%c", string_at(data, i));
    }

    fflush(fp);
    fclose(fp);

    return true;
}
