#include "question.h"

#include "config.h"

#include <utillib/core.h>
#include <utillib/utils.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_BUFFER_SIZE 256

static void get_user_input(array_t *data, char *prompt){
    fprintf(stdout, "%s: ", prompt);
    fflush(stdout);

    char *ret = fgets(array_get_data(data), array_get_size(data), stdin);

    if(ret != array_get_data(data))
        error("Failed to read input from stdin!");
}

static void sanitize(array_t *in, array_t *out){
    unsigned out_count = 0;

    array_cleanup(out);

    for(unsigned i = 0; *(char *)array_at(in, i) != '\0'; i++){
        char c = *(char *)array_at(in, i);

        if(isprint(c)){
            if(out_count >= array_get_size(out))
                array_enlarge(out);

            array_set(out, out_count++, (void *)&c);
        }
        else if(c == '\n' || c == '\r'){
            char tmp = '\0';

            if(out_count >= array_get_size(out))
                array_enlarge(out);

            array_set(out, out_count++, (void *)&tmp);
            break;
        }
        else if(c == '\t'){
            char tmp = ' ';

            for(int y = 0; y < DEFAULT_CLI_TAB_SIZE; y++){
                if(out_count >= array_get_size(out))
                    array_enlarge(out);

                array_set(out, out_count++, (void *)&tmp);
            }
        }
        else{
            error("Found forbidden character in user input!");
        }
    }
}

static bool _question(char *question, char *select, bool enable_default, bool default_value){
    bool ret_val = default_value;

    array_t *response_array = NULL;
    array_t *cleanup_array = NULL;

    array_init(&response_array, sizeof(char), DEFAULT_BUFFER_SIZE);
    array_init(&cleanup_array, sizeof(char), DEFAULT_BUFFER_SIZE);

    unsigned long len = strlen(question) + strlen(select) + 2;

    while(1) {
        array_cleanup(response_array);
        array_cleanup(cleanup_array);

        char *prompt = dynmem_calloc(len, sizeof(char));

        strcpy(prompt, question);
        strcat(prompt, " ");
        strcat(prompt, select);

        get_user_input(response_array, prompt);

        dynmem_free(prompt);

        sanitize(response_array, cleanup_array);

        for(unsigned i = 0; *(char *)array_at(cleanup_array, i) != '\0'; i++){
            char tmp = *(char *)array_at(cleanup_array, i);
            tmp = tolower(tmp);
            array_set(cleanup_array, i, (void *)&tmp);
        }

        char *response = (char *)array_get_data(cleanup_array);

        if((strcmp(response, "yes") == 0) || (strcmp(response, "y") == 0)){
            ret_val = true;
            break;
        }

        if((strcmp(response, "no") == 0) || (strcmp(response, "n") == 0)){
            ret_val = false;
            break;
        }

        if(enable_default && strcmp(response, "") == 0){
            break;
        }

        fprintf(stdout, "Can't decode your input. Please try again.\r\n");
    }

    array_destroy(response_array);
    array_destroy(cleanup_array);

    return ret_val;
}

bool question_yes_no(char *s){
    return _question(s, "[y/n]", false, false);
}

bool question_YES_no(char *s){
    return _question(s, "[Y/n]", true, true);
}

bool question_yes_NO(char *s){
    return _question(s, "[y/N]", true, false);
}

char *user_input_string(char *s){
    array_t *response_array = NULL;
    array_t *cleanup_array = NULL;

    array_init(&response_array, sizeof(char), DEFAULT_BUFFER_SIZE);
    array_init(&cleanup_array, sizeof(char), DEFAULT_BUFFER_SIZE);

    array_cleanup(response_array);
    array_cleanup(cleanup_array);

    get_user_input(response_array, s);

    sanitize(response_array, cleanup_array);

    char *tmp = dynmem_strdup(array_get_data(cleanup_array));

    array_destroy(response_array);
    array_destroy(cleanup_array);

    return tmp;
}

long long user_input_number(char *s){
    char *ret = NULL;
    long long x = 0;

    while(true){
        ret = user_input_string(s);

        if(is_number(ret) == false){
            fprintf(stderr, "Entered something that isn't number! Try again!\r\n");
            fflush(stderr);
            dynmem_free(ret);
            continue;
        }
        else{
            break;
        }
    }

    if(str_to_num(ret, &x) == false)
        error("Cannot convert string to number!");

    dynmem_free(ret);

    return x;
}
