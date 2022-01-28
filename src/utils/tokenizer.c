#include "tokenizer.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <utillib/core.h>

#define CHUNK_SIZE_FOR_READING_FILE 1024

static list_t *filename_cache = NULL;

static token_t *tokenizer_token_new(char *token_data, char *filename, long line_number, long column);
static char *filename_store(char *filename);
static void clean_filename_cache(void);
static void handle_two_char_comment(tokenizer_t *this);

bool is_separator(tokenizer_t *this){
    switch(this->state.current_char){
        case ' ':
        case '\n':
            return true;
        default:
            return false;
    }
}

static bool is_newline(tokenizer_t *this){
    if(this->state.current_char == '\n')
        return true;
    else
        return false;
}

bool is_comment_start(tokenizer_t *this){
    if(this->state.clike_comments.enabled == true){
        if(this->state.current_char == '*' && this->state.previous_char == '/'){
            this->state.previous_char_was_comment_mark = true;
            this->state.clike_comments.multiline_active = true;
            return true;
        }
    }

    if(this->state.current_char == '/' && this->state.previous_char == '/'){
        this->state.previous_char_was_comment_mark = true;
        return true;
    }
    else{
        return false;
    }
}

bool is_comment_end(tokenizer_t *this){
    if(this->state.clike_comments.multiline_active == true){
        if(this->state.current_char == '/' && this->state.previous_char == '*'){
            this->state.previous_char_was_comment_mark = true;
            this->state.clike_comments.multiline_active = false;
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return is_newline(this);
    }
}

static bool should_skip(tokenizer_t *this){
    switch(this->state.current_char){
        case '\r':
            return true;
        default:
            return false;
    }
}

static void replace_if_needed(tokenizer_t *this){
    if(this->state.current_char == '\t'){
        this->state.current_char = ' ';
    }
}

static bool is_parenthessis(tokenizer_t *this){
    switch(this->state.current_char){
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
            return true;
        default:
            return false;
    }
}

static bool is_string_mark(tokenizer_t *this){
    if(this->state.current_char == '\"' && this->state.previous_char != '\\')
        return true;
    else if(this->state.current_char == '\'' && this->state.previous_char != '\\')
        return true;
    else
        return false;
}

static void append_in_buffer(tokenizer_t *this){
    unsigned len = strlen((char *)array_get_data(this->buffer));
    char tmp = '\0';

    while(len + 2 > array_get_size(this->buffer)){
        array_enlarge(this->buffer);
    }

    array_set(this->buffer, len, (void *)&(this->state.current_char));
    array_set(this->buffer, len + 1, (void *)&tmp);
}

static void put_buffer_to_output(tokenizer_t *this){
    char *data = NULL;
    unsigned data_len = 0;

    data = (char *)array_get_data(this->buffer);
    data_len = strlen(data);

    if(data_len == 0)
        return;

    token_t *tmp = tokenizer_token_new(data,
        this->state.current_filename,
        this->state.current_line_number,
        this->state.current_column_number - data_len
    );

    queue_append(this->output, (void *)&tmp);
}

static void cleanup_buffer(tokenizer_t *this){
    char tmp = '\0';

    array_cleanup(this->buffer);
    array_set(this->buffer, 0, (void *)&tmp);   //make sure there is null char
}

static void tokenize_loop(tokenizer_t *tokenizer, char *input){
    tokenizer_t *t = tokenizer;

    for(unsigned i = 0; input[i] != '\0'; i++){
        t->state.current_char = input[i];

        replace_if_needed(t);

        if(should_skip(t))
            continue;

        if(t->state.comment_block_active){
            if(t->methods.is_comment_end(t)){
                t->state.comment_block_active = false;
            }
        }
        else{
            if(t->methods.is_comment_start(t)){
                t->state.comment_block_active = true;
                handle_two_char_comment(t);
            }
            else if(is_string_mark(t)){
                append_in_buffer(t);
                if(t->state.string_block_active){
                    t->state.string_block_active = false;
                    put_buffer_to_output(t);
                    cleanup_buffer(t);
                }
                else{
                    t->state.string_block_active = true;
                }
            }
            else if(is_parenthessis(t)){
                if(t->state.string_block_active){
                    append_in_buffer(t);
                }
                else{
                    put_buffer_to_output(t);
                    cleanup_buffer(t);

                    append_in_buffer(t);

                    put_buffer_to_output(t);
                    cleanup_buffer(t);
                }
            }
            else if(t->methods.is_separator(t)){
                if(t->state.string_block_active){
                    append_in_buffer(t);
                }
                else{
                    put_buffer_to_output(t);
                    cleanup_buffer(t);
                }
            }
            else{
                append_in_buffer(t);
            }
        }

        if(is_newline(t)){
            t->state.current_line_number += 1;
            t->state.current_column_number = 1;
        }
        else{
            t->state.current_column_number += 1;
        }

        t->state.previous_char = t->state.current_char;
    }
}

void tokenizer_init(tokenizer_t **tokenizer){
    CHECK_NULL_ARGUMENT(tokenizer);
    CHECK_NOT_NULL_ARGUMENT(*tokenizer);

    tokenizer_t *tmp = (tokenizer_t *)dynmem_malloc(sizeof(tokenizer_t));

    tmp->state.comment_block_active = false;
    tmp->state.previous_char_was_comment_mark = false;
    tmp->state.string_block_active = false;
    tmp->state.current_char = '\0';
    tmp->state.previous_char = '\0';
    tmp->state.current_line_number = 1;
    tmp->state.current_column_number = 1;
    tmp->state.current_filename = "";

    tmp->methods.is_comment_end = is_comment_end;
    tmp->methods.is_comment_start = is_comment_start;
    tmp->methods.is_separator = is_separator;

    tmp->buffer = NULL;
    tmp->output = NULL;

    array_init(&(tmp->buffer), sizeof(char), 32);
    queue_init(&(tmp->output), sizeof(token_t *));

    array_cleanup(tmp->buffer);

    *tokenizer = tmp;
}

void tokenizer_end(tokenizer_t *tokenizer, queue_t **output){
    CHECK_NULL_ARGUMENT(tokenizer);
    CHECK_NULL_ARGUMENT(output);

    put_buffer_to_output(tokenizer);

    if(*output == NULL)
        queue_copy(tokenizer->output, output);
    else
        queue_merge(tokenizer->output, *output);

    queue_destroy(tokenizer->output);
    array_destroy(tokenizer->buffer);
    dynmem_free(tokenizer);
}

void tokenizer_tokenize_string(tokenizer_t *tokenizer, string_t *input){
    CHECK_NULL_ARGUMENT(tokenizer);
    CHECK_NULL_ARGUMENT(input);

    tokenize_loop(tokenizer, string_get(input));
}

void tokenizer_tokenize_char_string(tokenizer_t *tokenizer, char *input){
    CHECK_NULL_ARGUMENT(tokenizer);
    CHECK_NULL_ARGUMENT(input);

    tokenize_loop(tokenizer, input);
}

bool tokenizer_tokenize_file(tokenizer_t *tokenizer, char *filename){
    CHECK_NULL_ARGUMENT(tokenizer);
    CHECK_NULL_ARGUMENT(filename);

    FILE *fp = fopen(filename, "rb");

    if(fp == NULL)
        return false;

    char *tmp_buffer = dynmem_calloc(CHUNK_SIZE_FOR_READING_FILE, sizeof(char));
    size_t read = 0;

    tokenizer->state.current_filename = filename;

    do{
        read = fread((void *)tmp_buffer, sizeof(char), CHUNK_SIZE_FOR_READING_FILE - 1, fp);
        tmp_buffer[read] = '\0';
        tokenize_loop(tokenizer, tmp_buffer);
    }while(read > 0);

    dynmem_free(tmp_buffer);
    fclose(fp);

    tokenizer->state.current_filename = "";

    return true;
}

void tokenizer_config_comment(
    tokenizer_t *tokenizer,
    bool (*is_comment_start)(tokenizer_t *this),
    bool (*is_comment_end)(tokenizer_t *this)
){
    CHECK_NULL_ARGUMENT(tokenizer);
    CHECK_NULL_ARGUMENT(is_comment_start);
    CHECK_NULL_ARGUMENT(is_comment_end);

    tokenizer->methods.is_comment_end = is_comment_end;
    tokenizer->methods.is_comment_start = is_comment_start;
}

void tokenizer_config_enable_c_like_comment(
    tokenizer_t *tokenizer
)
{
    CHECK_NULL_ARGUMENT(tokenizer);

    tokenizer->state.clike_comments.enabled = true;
}

void tokenizer_config_separator(
    tokenizer_t *tokenizer,
    bool (*is_separator)(tokenizer_t *this)
){
    CHECK_NULL_ARGUMENT(tokenizer);
    CHECK_NULL_ARGUMENT(is_separator);

    tokenizer->methods.is_separator = is_separator;
}

void tokenizer_clean_output_queue(queue_t *output){
    CHECK_NULL_ARGUMENT(output);

    while(queue_count(output) > 0){
        token_t *tmp = NULL;
        queue_windraw(output, (void *)&tmp);
        tokenizer_token_destroy(tmp);
    }

    queue_destroy(output);
}

static token_t *tokenizer_token_new(char *token_data, char *filename, long line_number, long column){
    CHECK_NULL_ARGUMENT(token_data);
    CHECK_NULL_ARGUMENT(filename);

    token_t *tmp = (token_t *)dynmem_malloc(sizeof(token_t));

    tmp->token = dynmem_strdup(token_data);
    tmp->column = column;
    tmp->filename = filename_store(filename);
    tmp->line_number = line_number;

    return tmp;
}

void tokenizer_token_destroy(token_t *token){
    CHECK_NULL_ARGUMENT(token);

    if(token->token != NULL){
        dynmem_free(token->token);
    }

    dynmem_free(token);
}

static char *filename_store(char *filename){
    CHECK_NULL_ARGUMENT(filename);

    if(filename_cache == NULL){
        list_init(&filename_cache, sizeof(char *));
        atexit_register(&clean_filename_cache);
    }

    for(unsigned int i = 0; i < list_count(filename_cache); i++){
        char *filename_cache_head = NULL;
        list_at(filename_cache, i, (void *)&filename_cache_head);

        if(strcmp(filename_cache_head, filename) == 0){
            return filename_cache_head;
        }
    }

    char *tmp = dynmem_strdup(filename);

    list_append(filename_cache, (void *)&tmp);

    return tmp;
}

static void clean_filename_cache(void){
    if(filename_cache != NULL){
        while(queue_count(filename_cache) > 0){
            char *tmp = NULL;
            queue_windraw(filename_cache, (void *)&tmp);
            dynmem_free(tmp);
        }
        list_destroy(filename_cache);
        filename_cache = NULL;
    }
}

static void handle_two_char_comment(tokenizer_t *this){
    CHECK_NULL_ARGUMENT(this);

    if(this->state.previous_char_was_comment_mark == false)
        return;

    unsigned pos = 0;
    char tmp = '\0';

    while('\0' != *(char *)array_at(this->buffer, pos)){
        pos++;
    }

    array_set(this->buffer, pos - 1, (void *)&tmp);
    this->state.previous_char_was_comment_mark = false;
}
